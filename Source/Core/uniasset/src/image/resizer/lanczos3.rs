pub fn lanczos3_dispatch(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
) {
    // Only pixel sizes {1, 3, 4} are considered for SIMD fast paths.
    match pixel_size {
        1 | 3 | 4 => {}
        _ => {
            lanczos3_generic(
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

    #[cfg(target_arch = "aarch64")]
    {
        if std::arch::is_aarch64_feature_detected!("neon") {
            unsafe {
                if lanczos3_neon(
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

    lanczos3_generic(
        input,
        output,
        origin_width,
        origin_height,
        dest_width,
        dest_height,
        pixel_size,
    );
}

fn lanczos3_generic(
    input: &[u8],
    output: &mut [u8],
    src_w: u32,
    src_h: u32,
    dst_w: u32,
    dst_h: u32,
    pixel_size: u32,
) {
    match pixel_size as usize {
        1 => resize_const::<1>(input, output, src_w, src_h, dst_w, dst_h),
        3 => resize_const::<3>(input, output, src_w, src_h, dst_w, dst_h),
        4 => resize_const::<4>(input, output, src_w, src_h, dst_w, dst_h),
        _ => resize_dynamic(input, output, src_w, src_h, dst_w, dst_h, pixel_size),
    }
}

fn resize_const<const PS: usize>(
    input: &[u8],
    output: &mut [u8],
    src_w: u32,
    src_h: u32,
    dst_w: u32,
    dst_h: u32,
) {
    let w_contribs = precompute_weights(src_w, dst_w);
    let h_contribs = precompute_weights(src_h, dst_h);

    for y in 0..dst_h {
        let h_contrib = &h_contribs[y as usize];
        let dst_row_offset = (y * dst_w) as usize * PS;

        for x in 0..dst_w {
            let w_contrib = &w_contribs[x as usize];
            let mut sums = [0.0f32; PS];

            for ky in 0..6 {
                let py = h_contrib.start + ky;
                let wy = h_contrib.weights[ky];
                let src_row_offset = (py * src_w as usize) * PS;

                for kx in 0..6 {
                    let px = w_contrib.start + kx;
                    let wx = w_contrib.weights[kx];
                    let weight = wy * wx;

                    let pixel_idx = src_row_offset + (px * PS);
                    for c in 0..PS {
                        sums[c] += input[pixel_idx + c] as f32 * weight;
                    }
                }
            }

            let out_idx = dst_row_offset + (x as usize * PS);
            for c in 0..PS {
                output[out_idx + c] = sums[c].clamp(0.0, 255.0) as u8;
            }
        }
    }
}

fn resize_dynamic(
    input: &[u8],
    output: &mut [u8],
    src_w: u32,
    src_h: u32,
    dst_w: u32,
    dst_h: u32,
    pixel_size: u32,
) {
    let channels = pixel_size as usize;
    let w_contribs = precompute_weights(src_w, dst_w);
    let h_contribs = precompute_weights(src_h, dst_h);

    let src_stride = src_w as usize * channels;
    let dst_stride = dst_w as usize * channels;

    for y in 0..dst_h {
        let h_contrib = &h_contribs[y as usize];
        let dst_row_ptr = y as usize * dst_stride;

        for x in 0..dst_w {
            let w_contrib = &w_contribs[x as usize];
            let dst_pixel_ptr = dst_row_ptr + (x as usize * channels);

            let mut sums = [0.0f32; 32];
            let active_sums = &mut sums[..channels];

            for ky in 0..6 {
                let py = h_contrib.start + ky;
                let wy = h_contrib.weights[ky];
                let src_row_ptr = py * src_stride;

                for kx in 0..6 {
                    let px = w_contrib.start + kx;
                    let wx = w_contrib.weights[kx];
                    let weight = wy * wx;

                    let src_pixel_ptr = src_row_ptr + (px * channels);
                    for c in 0..channels {
                        active_sums[c] += input[src_pixel_ptr + c] as f32 * weight;
                    }
                }
            }

            for c in 0..channels {
                output[dst_pixel_ptr + c] = active_sums[c].clamp(0.0, 255.0) as u8;
            }
        }
    }
}

fn lanczos3_kernel(x: f32) -> f32 {
    let x = x.abs();
    if x < 1e-6 {
        return 1.0;
    }
    if x >= 3.0 {
        return 0.0;
    }

    let pi_x = x * std::f32::consts::PI;
    let sinc = |a: f32| a.sin() / a;
    sinc(pi_x) * sinc(pi_x / 3.0)
}

#[derive(Clone)]
struct Contribution {
    start: usize,
    weights: [f32; 6],
}

fn precompute_weights(src_size: u32, dst_size: u32) -> Vec<Contribution> {
    let scale = src_size as f32 / dst_size as f32;
    (0..dst_size)
        .map(|i| {
            let center = (i as f32 + 0.5) * scale;
            let start = (center - 3.0).ceil() as i32;
            let mut contrib = Contribution {
                start: 0,
                weights: [0.0; 6],
            };
            let mut sum = 0.0;

            for j in 0..6 {
                let pos = start + j as i32;
                let weight = lanczos3_kernel((pos as f32 + 0.5) - center);
                contrib.weights[j] = weight;
                sum += weight;
                if j == 0 {
                    contrib.start = pos.clamp(0, src_size as i32 - 6) as usize;
                }
            }
            for w in &mut contrib.weights {
                *w /= sum;
            }
            contrib
        })
        .collect()
}

// ----------------------------- aarch64 NEON -----------------------------
#[cfg(target_arch = "aarch64")]
mod arm_neon {
    use super::precompute_weights;
    use std::arch::aarch64::*;

    #[target_feature(enable = "neon")]
    pub unsafe fn lanczos3_neon(
        input: &[u8],
        output: &mut [u8],
        src_w: u32,
        src_h: u32,
        dst_w: u32,
        dst_h: u32,
        pixel_size: u32,
    ) -> bool {
        if pixel_size != 4 {
            return false;
        }

        let w_contribs = precompute_weights(src_w, dst_w);
        let h_contribs = precompute_weights(src_h, dst_h);
        let src_stride = (src_w * 4) as usize;
        let dst_stride = (dst_w * 4) as usize;

        for y in 0..dst_h {
            let h_contrib = &h_contribs[y as usize];
            let dst_row_ptr = unsafe { output.as_mut_ptr().add(y as usize * dst_stride) };

            for x in 0..dst_w {
                let w_contrib = &w_contribs[x as usize];
                let mut acc = vdupq_n_f32(0.0);

                for ky in 0..6 {
                    let py = h_contrib.start + ky;
                    let wy = vdupq_n_f32(h_contrib.weights[ky]);
                    let src_row_ptr = unsafe { input.as_ptr().add(py * src_stride) };

                    for kx in 0..6 {
                        let px = w_contrib.start + kx;
                        let wx = vdupq_n_f32(w_contrib.weights[kx]);
                        let combined_weight = vmulq_f32(wy, wx);

                        let pixel_ptr = unsafe { src_row_ptr.add(px * 4) };
                        let raw_pixel = unsafe { vld1_u8(pixel_ptr) };

                        let u16_pixel = vmovl_u8(raw_pixel);
                        let u32_pixel = vmovl_u16(vget_low_u16(u16_pixel));
                        let f32_pixel = vcvtq_f32_u32(u32_pixel);

                        acc = vfmaq_f32(acc, f32_pixel, combined_weight);
                    }
                }

                let res_u32 = vcvtq_u32_f32(acc);
                let res_u16 = vqmovn_u32(res_u32);
                let res_u8 = vqmovn_u16(vcombine_u16(res_u16, res_u16));

                let out_pixel_ptr = unsafe { dst_row_ptr.add(x as usize * 4) } as *mut u32;
                unsafe {
                    std::ptr::write_unaligned(
                        out_pixel_ptr,
                        vget_lane_u32(vreinterpret_u32_u8(res_u8), 0),
                    )
                };
            }
        }
        true
    }
}

#[cfg(target_arch = "aarch64")]
use arm_neon::lanczos3_neon;
