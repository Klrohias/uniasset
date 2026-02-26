#[cfg(target_arch = "aarch64")]
use std::arch::aarch64::*;

#[cfg(any(target_arch = "x86", target_arch = "x86_64"))]
use std::arch::x86_64::*;

pub fn box_resize_dispatch(
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
        use std::arch::is_aarch64_feature_detected;

        if is_aarch64_feature_detected!("neon") {
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
unsafe fn resize_neon(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    match pixel_size {
        1 => unsafe {
            resize_neon_specialized::<1>(
                input,
                output,
                origin_width,
                origin_height,
                dest_width,
                dest_height,
            )
        },
        3 => unsafe {
            resize_neon_specialized::<3>(
                input,
                output,
                origin_width,
                origin_height,
                dest_width,
                dest_height,
            )
        },
        4 => unsafe {
            resize_neon_specialized::<4>(
                input,
                output,
                origin_width,
                origin_height,
                dest_width,
                dest_height,
            )
        },
        _ => resize_fallback(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
            pixel_size,
        ),
    }
}

#[cfg(target_arch = "aarch64")]
unsafe fn resize_neon_specialized<const PS: usize>(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
) {
    let src_w = origin_width as usize;
    let src_h = origin_height as usize;
    let dst_w = dest_width as usize;
    let dst_h = dest_height as usize;

    let scale_x = src_w as f32 / dst_w as f32;
    let scale_y = src_h as f32 / dst_h as f32;

    let src_stride = src_w * PS;
    let dst_stride = dst_w * PS;

    for dy in 0..dst_h {
        let y0 = (dy as f32 * scale_y).floor() as usize;
        let y1 = ((dy as f32 + 1.0) * scale_y).ceil() as usize;
        let y1 = y1.min(src_h);

        for dx in 0..dst_w {
            let x0 = (dx as f32 * scale_x).floor() as usize;
            let x1 = ((dx as f32 + 1.0) * scale_x).ceil() as usize;
            let x1 = x1.min(src_w);

            let mut count = 0u32;

            let mut acc = [0u32; 4];

            for sy in y0..y1 {
                let row = unsafe { input.as_ptr().add(sy * src_stride) };

                let mut sx = x0;
                while sx + 16 <= x1 {
                    let ptr = unsafe { row.add(sx * PS) };

                    match PS {
                        1 => {
                            let v = unsafe { vld1q_u8(ptr) };
                            let lo = unsafe { vmovl_u8(vget_low_u8(v)) };
                            let hi = unsafe { vmovl_u8(vget_high_u8(v)) };
                            let sum = unsafe { vaddq_u16(lo, hi) };
                            acc[0] += unsafe { vaddvq_u16(sum) } as u32;
                            count += 16;
                        }
                        3 | 4 => {
                            for i in 0..PS {
                                let mut tmp = [0u8; 16];
                                for j in 0..16 {
                                    tmp[j] = unsafe { *ptr.add(j * PS + i) };
                                }
                                let v = unsafe { vld1q_u8(tmp.as_ptr()) };
                                let lo = unsafe { vmovl_u8(vget_low_u8(v)) };
                                let hi = unsafe { vmovl_u8(vget_high_u8(v)) };
                                acc[i] += unsafe { vaddvq_u16(vaddq_u16(lo, hi)) } as u32;
                            }
                            count += 16;
                        }
                        _ => unreachable!(),
                    }

                    sx += 16;
                }

                while sx < x1 {
                    let p = unsafe { row.add(sx * PS) };
                    for c in 0..PS {
                        acc[c] += unsafe { *p.add(c) } as u32;
                    }
                    count += 1;
                    sx += 1;
                }
            }

            let dst = unsafe { output.as_mut_ptr().add(dy * dst_stride + dx * PS) };
            let inv = 1.0 / count as f32;

            for c in 0..PS {
                let v = (acc[c] as f32 * inv + 0.5) as u32;
                unsafe { *dst.add(c) = v.min(255) as u8 };
            }
        }
    }
}

fn resize_fallback(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    match pixel_size {
        1 => resize_specialized::<1>(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
        ),
        3 => resize_specialized::<3>(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
        ),
        4 => resize_specialized::<4>(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
        ),
        _ => resize_generic(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
            pixel_size,
        ),
    }
}

fn resize_specialized<const PS: usize>(
    input: &[u8],
    output: &mut [u8],
    ow: u32,
    oh: u32,
    dw: u32,
    dh: u32,
) {
    let x_ratio = (ow as f64) / (dw as f64);
    let y_ratio = (oh as f64) / (dh as f64);

    for dy in 0..dh {
        let sy_start = (dy as f64 * y_ratio) as u32;
        let sy_end = (((dy + 1) as f64 * y_ratio) as u32)
            .min(oh)
            .max(sy_start + 1);
        let row_count = sy_end - sy_start;

        for dx in 0..dw {
            let sx_start = (dx as f64 * x_ratio) as u32;
            let sx_end = (((dx + 1) as f64 * x_ratio) as u32)
                .min(ow)
                .max(sx_start + 1);
            let col_count = sx_end - sx_start;

            let pixel_count = (row_count * col_count) as u32;
            let mut sums = [0u32; PS];

            for sy in sy_start..sy_end {
                let row_offset = (sy * ow + sx_start) as usize * PS;
                let row_data = &input[row_offset..row_offset + (col_count as usize * PS)];

                for x in 0..col_count as usize {
                    for c in 0..PS {
                        sums[c] += row_data[x * PS + c] as u32;
                    }
                }
            }

            let out_idx = (dy * dw + dx) as usize * PS;
            for c in 0..PS {
                output[out_idx + c] = (sums[c] / pixel_count) as u8;
            }
        }
    }
}

fn resize_generic(input: &[u8], output: &mut [u8], ow: u32, oh: u32, dw: u32, dh: u32, ps: u32) {
    let ps = ps as usize;
    let x_ratio = ow as f64 / dw as f64;
    let y_ratio = oh as f64 / dh as f64;

    for dy in 0..dh {
        let sy_start = (dy as f64 * y_ratio) as u32;
        let sy_end = (((dy + 1) as f64 * y_ratio) as u32)
            .min(oh)
            .max(sy_start + 1);

        for dx in 0..dw {
            let sx_start = (dx as f64 * x_ratio) as u32;
            let sx_end = (((dx + 1) as f64 * x_ratio) as u32)
                .min(ow)
                .max(sx_start + 1);

            let mut sums = vec![0u32; ps];
            for sy in sy_start..sy_end {
                for sx in sx_start..sx_end {
                    let idx = (sy * ow + sx) as usize * ps;
                    for c in 0..ps {
                        sums[c] += input[idx + c] as u32;
                    }
                }
            }

            let area = (sy_end - sy_start) * (sx_end - sx_start);
            let out_idx = (dy * dw + dx) as usize * ps;
            for c in 0..ps {
                output[out_idx + c] = (sums[c] / area) as u8;
            }
        }
    }
}
