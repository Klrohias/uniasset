#[cfg(target_arch = "aarch64")]
use std::arch::aarch64::*;

pub fn nearest_resize_dispatch(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    #[cfg(target_arch = "aarch64")]
    {
        if std::arch::is_aarch64_feature_detected!("neon") {
            return unsafe {
                resize_neon(
                    input,
                    output,
                    origin_width,
                    origin_height,
                    dest_width,
                    dest_height,
                    pixel_size,
                )
            };
        }
    }

    resize_fallback(
        input,
        output,
        origin_width,
        origin_height,
        dest_width,
        dest_height,
        pixel_size,
    );
}

#[cfg(target_arch = "aarch64")]
#[target_feature(enable = "neon")]
fn resize_neon(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    let scale_x = origin_width as f32 / dest_width as f32;
    let scale_y = origin_height as f32 / dest_height as f32;

    let x_offsets: Vec<usize> = (0..dest_width)
        .map(|dx| {
            ((dx as f32 * scale_x) as usize).min(origin_width as usize - 1) * pixel_size as usize
        })
        .collect();

    if pixel_size == 4 {
        for dy in 0..dest_height {
            let sy = ((dy as f32 * scale_y) as usize).min(origin_height as usize - 1);
            let src_row_ptr = unsafe { input.as_ptr().add(sy * origin_width as usize * 4) };
            let dst_row_ptr = unsafe {
                output
                    .as_mut_ptr()
                    .add(dy as usize * dest_width as usize * 4)
            };

            let mut dx = 0;

            while dx + 3 < dest_width as usize {
                let ox0 = x_offsets[dx];
                let ox1 = x_offsets[dx + 1];
                let ox2 = x_offsets[dx + 2];
                let ox3 = x_offsets[dx + 3];

                let p0 = unsafe { *(src_row_ptr.add(ox0) as *const u32) };
                let p1 = unsafe { *(src_row_ptr.add(ox1) as *const u32) };
                let p2 = unsafe { *(src_row_ptr.add(ox2) as *const u32) };
                let p3 = unsafe { *(src_row_ptr.add(ox3) as *const u32) };

                let v = vsetq_lane_u32(
                    p3,
                    vsetq_lane_u32(p2, vsetq_lane_u32(p1, vmovq_n_u32(p0), 1), 2),
                    3,
                );

                unsafe { vst1q_u8(dst_row_ptr.add(dx * 4), vreinterpretq_u8_u32(v)) };
                dx += 4;
            }

            while dx < dest_width as usize {
                let ox = x_offsets[dx];
                unsafe {
                    *(dst_row_ptr.add(dx * 4) as *mut u32) = *(src_row_ptr.add(ox) as *const u32)
                };
                dx += 1;
            }
        }
    } else {
        resize_fallback(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
            pixel_size,
        );
    }
}

fn resize_fallback(input: &[u8], output: &mut [u8], ow: u32, oh: u32, dw: u32, dh: u32, ps: u32) {
    let scale_x = ow as f32 / dw as f32;
    let scale_y = oh as f32 / dh as f32;

    let x_offsets: Vec<usize> = (0..dw)
        .map(|dx| ((dx as f32 * scale_x) as usize).min(ow as usize - 1) * ps as usize)
        .collect();

    match ps {
        1 => process_rows::<1>(input, output, ow, dw, dh, scale_y, &x_offsets),
        3 => process_rows::<3>(input, output, ow, dw, dh, scale_y, &x_offsets),
        4 => process_rows::<4>(input, output, ow, dw, dh, scale_y, &x_offsets),
        _ => process_rows_generic(input, output, ow, dw, dh, ps, scale_y, &x_offsets),
    }
}

#[inline(always)]
fn process_rows<const PS: usize>(
    input: &[u8],
    output: &mut [u8],
    ow: u32,
    dw: u32,
    dh: u32,
    scale_y: f32,
    x_offsets: &[usize],
) {
    for dy in 0..dh {
        let sy = ((dy as f32 * scale_y) as usize).min(ow as usize - 1);
        let src_row = &input[sy * ow as usize * PS..];
        let dst_row = &mut output[dy as usize * dw as usize * PS..];

        for dx in 0..dw as usize {
            let src_idx = x_offsets[dx];
            let dst_idx = dx * PS;

            for b in 0..PS {
                dst_row[dst_idx + b] = src_row[src_idx + b];
            }
        }
    }
}

fn process_rows_generic(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
    scale_y: f32,
    x_indices: &[usize],
) {
    let ps = pixel_size as usize;
    for dy in 0..dest_height {
        let sy = (dy as f32 * scale_y) as u32;
        let src_row_start = sy as usize * origin_width as usize * ps;
        let dst_row_start = dy as usize * dest_width as usize * ps;

        for dx in 0..dest_width as usize {
            let src_idx = src_row_start + x_indices[dx];
            let dst_idx = dst_row_start + dx * ps;
            for b in 0..ps {
                output[dst_idx + b] = input[src_idx + b];
            }
        }
    }
}
