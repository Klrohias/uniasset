pub fn nearest_resize_dispatch(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    // Only pixel sizes {1, 3, 4} are considered for SIMD fast paths.
    // Everything else uses the generic implementation.
    match pixel_size {
        1 | 3 | 4 => {}
        _ => {
            nearest_resize_generic(
                input,
                output,
                origin_width,
                origin_height,
                dest_width,
                dest_height,
                pixel_size,
            );
            return;
        }
    }

    #[cfg(all(target_arch = "aarch64"))]
    {
        if std::arch::is_aarch64_feature_detected!("neon") {
            unsafe {
                if nearest_resize_neon(
                    input,
                    output,
                    origin_width,
                    origin_height,
                    dest_width,
                    dest_height,
                    pixel_size,
                ) {
                    return;
                }
            }
        }
    }

    #[cfg(all(target_arch = "x86_64"))]
    {
        if std::arch::is_x86_feature_detected!("sse2") {
            unsafe {
                if nearest_resize_x86_sse2(
                    input,
                    output,
                    origin_width,
                    origin_height,
                    dest_width,
                    dest_height,
                    pixel_size,
                ) {
                    return;
                }
            }
        }
    }

    nearest_resize_generic(
        input,
        output,
        origin_width,
        origin_height,
        dest_width,
        dest_height,
        pixel_size,
    );
}

#[inline(always)]
fn build_x_offsets(ow: u32, dw: u32, ps: usize) -> Vec<usize> {
    let scale_x = ow as f32 / dw as f32;
    let max_x = ow as usize - 1;
    (0..dw as usize)
        .map(|dx| (((dx as f32 * scale_x) as usize).min(max_x)) * ps)
        .collect()
}

#[inline(always)]
fn src_y(oh: u32, dh: u32, dy: u32) -> usize {
    let scale_y = oh as f32 / dh as f32;
    let max_y = oh as usize - 1;
    ((dy as f32 * scale_y) as usize).min(max_y)
}

fn nearest_resize_generic(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    let ps = pixel_size as usize;
    let row_in = origin_width as usize * ps;
    let row_out = dest_width as usize * ps;
    let x_offsets = build_x_offsets(origin_width, dest_width, ps);

    match ps {
        1 => nearest_rows_const::<1>(
            input,
            output,
            row_in,
            row_out,
            origin_height,
            dest_height,
            &x_offsets,
        ),
        3 => nearest_rows_const::<3>(
            input,
            output,
            row_in,
            row_out,
            origin_height,
            dest_height,
            &x_offsets,
        ),
        4 => nearest_rows_const::<4>(
            input,
            output,
            row_in,
            row_out,
            origin_height,
            dest_height,
            &x_offsets,
        ),
        _ => nearest_rows_generic(
            input,
            output,
            row_in,
            row_out,
            origin_height,
            dest_height,
            ps,
            &x_offsets,
        ),
    }
}

#[inline(always)]
fn nearest_rows_const<const PS: usize>(
    input: &[u8],
    output: &mut [u8],
    row_in: usize,
    row_out: usize,
    origin_height: u32,
    dest_height: u32,
    x_offsets: &[usize],
) {
    let dw = (row_out / PS) as usize;
    for dy in 0..dest_height {
        let sy = src_y(origin_height, dest_height, dy) as usize;
        let src = &input[sy * row_in..sy * row_in + row_in];
        let dst = &mut output[dy as usize * row_out..dy as usize * row_out + row_out];
        for dx in 0..dw {
            let src_idx = x_offsets[dx];
            let dst_idx = dx * PS;
            let s = &src[src_idx..src_idx + PS];
            let d = &mut dst[dst_idx..dst_idx + PS];
            d.copy_from_slice(s);
        }
    }
}

#[inline(always)]
fn nearest_rows_generic(
    input: &[u8],
    output: &mut [u8],
    row_in: usize,
    row_out: usize,
    origin_height: u32,
    dest_height: u32,
    ps: usize,
    x_offsets: &[usize],
) {
    let dw = row_out / ps;
    for dy in 0..dest_height {
        let sy = src_y(origin_height, dest_height, dy) as usize;
        let src_row_start = sy * row_in;
        let dst_row_start = dy as usize * row_out;
        for dx in 0..dw {
            let src_idx = src_row_start + x_offsets[dx];
            let dst_idx = dst_row_start + dx * ps;
            output[dst_idx..dst_idx + ps].copy_from_slice(&input[src_idx..src_idx + ps]);
        }
    }
}

// ----------------------------- x86_64 SSE2 -----------------------------
#[cfg(target_arch = "x86_64")]
mod x86_sse2 {
    use super::{build_x_offsets, src_y};
    use std::arch::x86_64::*;

    #[target_feature(enable = "sse2")]
    pub unsafe fn nearest_resize_x86_sse2(
        input: &[u8],
        output: &mut [u8],
        origin_width: u32,
        origin_height: u32,
        dest_width: u32,
        dest_height: u32,
        pixel_size: u32,
    ) -> bool {
        match pixel_size {
            4 => unsafe {
                nearest_ps4(
                    input,
                    output,
                    origin_width,
                    origin_height,
                    dest_width,
                    dest_height,
                )
            },
            _ => false,
        }
    }

    #[inline(always)]
    unsafe fn nearest_ps4(
        input: &[u8],
        output: &mut [u8],
        origin_width: u32,
        origin_height: u32,
        dest_width: u32,
        dest_height: u32,
    ) -> bool {
        let ps = 4usize;
        let row_in = origin_width as usize * ps;
        let row_out = dest_width as usize * ps;
        let x_offsets = build_x_offsets(origin_width, dest_width, ps);

        for dy in 0..dest_height {
            let sy = src_y(origin_height, dest_height, dy) as usize;
            let src_row = unsafe { input.as_ptr().add(sy * row_in) };
            let dst_row = unsafe { output.as_mut_ptr().add(dy as usize * row_out) };

            let mut dx = 0usize;
            let dw = dest_width as usize;
            while dx + 4 <= dw {
                let o0 = x_offsets[dx];
                let o1 = x_offsets[dx + 1];
                let o2 = x_offsets[dx + 2];
                let o3 = x_offsets[dx + 3];
                let p0 = unsafe { *(src_row.add(o0) as *const u32) };
                let p1 = unsafe { *(src_row.add(o1) as *const u32) };
                let p2 = unsafe { *(src_row.add(o2) as *const u32) };
                let p3 = unsafe { *(src_row.add(o3) as *const u32) };

                let v = _mm_setr_epi32(p0 as i32, p1 as i32, p2 as i32, p3 as i32);
                unsafe { _mm_storeu_si128(dst_row.add(dx * 4) as *mut __m128i, v) };
                dx += 4;
            }

            while dx < dw {
                let o = x_offsets[dx];
                unsafe {
                    *(dst_row.add(dx * 4) as *mut u32) = *(src_row.add(o) as *const u32);
                }
                dx += 1;
            }
        }
        true
    }
}

#[cfg(target_arch = "x86_64")]
use x86_sse2::nearest_resize_x86_sse2;

// ----------------------------- aarch64 NEON -----------------------------
#[cfg(target_arch = "aarch64")]
mod arm_neon {
    use super::{build_x_offsets, src_y};
    use std::arch::aarch64::*;

    #[target_feature(enable = "neon")]
    pub unsafe fn nearest_resize_neon(
        input: &[u8],
        output: &mut [u8],
        origin_width: u32,
        origin_height: u32,
        dest_width: u32,
        dest_height: u32,
        pixel_size: u32,
    ) -> bool {
        match pixel_size {
            4 => unsafe {
                nearest_ps4(
                    input,
                    output,
                    origin_width,
                    origin_height,
                    dest_width,
                    dest_height,
                )
            },
            _ => false,
        }
    }

    #[inline(always)]
    unsafe fn nearest_ps4(
        input: &[u8],
        output: &mut [u8],
        origin_width: u32,
        origin_height: u32,
        dest_width: u32,
        dest_height: u32,
    ) -> bool {
        let ps = 4usize;
        let row_in = origin_width as usize * ps;
        let row_out = dest_width as usize * ps;
        let x_offsets = build_x_offsets(origin_width, dest_width, ps);

        for dy in 0..dest_height {
            let sy = src_y(origin_height, dest_height, dy) as usize;
            let src_row = unsafe { input.as_ptr().add(sy * row_in) };
            let dst_row = unsafe { output.as_mut_ptr().add(dy as usize * row_out) };

            let mut dx = 0usize;
            let dw = dest_width as usize;
            while dx + 4 <= dw {
                let o0 = x_offsets[dx];
                let o1 = x_offsets[dx + 1];
                let o2 = x_offsets[dx + 2];
                let o3 = x_offsets[dx + 3];
                let p0 = unsafe { *(src_row.add(o0) as *const u32) };
                let p1 = unsafe { *(src_row.add(o1) as *const u32) };
                let p2 = unsafe { *(src_row.add(o2) as *const u32) };
                let p3 = unsafe { *(src_row.add(o3) as *const u32) };

                let v = unsafe {
                    vsetq_lane_u32(
                        p3,
                        vsetq_lane_u32(p2, vsetq_lane_u32(p1, vmovq_n_u32(p0), 1), 2),
                        3,
                    )
                };
                unsafe { vst1q_u8(dst_row.add(dx * 4), vreinterpretq_u8_u32(v)) };
                dx += 4;
            }

            while dx < dw {
                let o = x_offsets[dx];
                unsafe {
                    *(dst_row.add(dx * 4) as *mut u32) = *(src_row.add(o) as *const u32);
                }
                dx += 1;
            }
        }

        true
    }
}

#[cfg(target_arch = "aarch64")]
use arm_neon::nearest_resize_neon;

// ----------------------------- tests -----------------------------
#[cfg(test)]
mod tests {
    use super::nearest_resize_dispatch;

    fn reference(input: &[u8], output: &mut [u8], ow: u32, oh: u32, dw: u32, dh: u32, ps: u32) {
        let psu = ps as usize;
        let scale_x = ow as f32 / dw as f32;
        let scale_y = oh as f32 / dh as f32;
        for dy in 0..dh {
            let sy = ((dy as f32 * scale_y) as usize).min(oh as usize - 1);
            for dx in 0..dw {
                let sx = ((dx as f32 * scale_x) as usize).min(ow as usize - 1);
                let s = (sy * ow as usize + sx) * psu;
                let d = (dy as usize * dw as usize + dx as usize) * psu;
                output[d..d + psu].copy_from_slice(&input[s..s + psu]);
            }
        }
    }

    #[test]
    fn nearest_ps1_matches_reference() {
        let (ow, oh, dw, dh, ps) = (7u32, 5u32, 11u32, 3u32, 1u32);
        let mut input = vec![0u8; (ow * oh * ps) as usize];
        for (i, v) in input.iter_mut().enumerate() {
            *v = (i as u8).wrapping_mul(17);
        }
        let mut out = vec![0u8; (dw * dh * ps) as usize];
        let mut exp = vec![0u8; out.len()];
        nearest_resize_dispatch(&input, &mut out, ow, oh, dw, dh, ps);
        reference(&input, &mut exp, ow, oh, dw, dh, ps);
        assert_eq!(out, exp);
    }

    #[test]
    fn nearest_ps3_matches_reference() {
        let (ow, oh, dw, dh, ps) = (8u32, 6u32, 5u32, 9u32, 3u32);
        let mut input = vec![0u8; (ow * oh * ps) as usize];
        for (i, v) in input.iter_mut().enumerate() {
            *v = (i as u8).wrapping_mul(31);
        }
        let mut out = vec![0u8; (dw * dh * ps) as usize];
        let mut exp = vec![0u8; out.len()];
        nearest_resize_dispatch(&input, &mut out, ow, oh, dw, dh, ps);
        reference(&input, &mut exp, ow, oh, dw, dh, ps);
        assert_eq!(out, exp);
    }

    #[test]
    fn nearest_ps4_matches_reference() {
        let (ow, oh, dw, dh, ps) = (9u32, 7u32, 13u32, 4u32, 4u32);
        let mut input = vec![0u8; (ow * oh * ps) as usize];
        for (i, v) in input.iter_mut().enumerate() {
            *v = (i as u8).wrapping_mul(13);
        }
        let mut out = vec![0u8; (dw * dh * ps) as usize];
        let mut exp = vec![0u8; out.len()];
        nearest_resize_dispatch(&input, &mut out, ow, oh, dw, dh, ps);
        reference(&input, &mut exp, ow, oh, dw, dh, ps);
        assert_eq!(out, exp);
    }

    #[test]
    fn nearest_other_pixel_size_falls_back() {
        let (ow, oh, dw, dh, ps) = (6u32, 6u32, 4u32, 4u32, 2u32);
        let mut input = vec![0u8; (ow * oh * ps) as usize];
        for (i, v) in input.iter_mut().enumerate() {
            *v = (i as u8).wrapping_mul(7);
        }
        let mut out = vec![0u8; (dw * dh * ps) as usize];
        let mut exp = vec![0u8; out.len()];
        nearest_resize_dispatch(&input, &mut out, ow, oh, dw, dh, ps);
        reference(&input, &mut exp, ow, oh, dw, dh, ps);
        assert_eq!(out, exp);
    }
}
