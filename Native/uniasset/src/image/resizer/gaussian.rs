#[cfg(target_arch = "aarch64")]
use std::arch::aarch64::*;

pub fn gaussian_dispatch(
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

unsafe fn resize_neon(
    input: &[u8],
    output: &mut [u8],
    src_w: u32,
    src_h: u32,
    dst_w: u32,
    dst_h: u32,
    pixel_size: u32,
) {
    if pixel_size != 4 {
        return resize_fallback(input, output, src_w, src_h, dst_w, dst_h, pixel_size);
    }
    let weights_h = precompute_gaussian_weights(src_w, dst_w, 1.0); // 示例 sigma=1.0
    let weights_v = precompute_gaussian_weights(src_h, dst_h, 1.0);

    let mut tmp = vec![0.0f32; (dst_w * src_h) as usize * 4];

    for y in 0..src_h {
        let src_row = &input[(y * src_w) as usize * 4..];
        let tmp_row = &mut tmp[(y * dst_w) as usize * 4..];

        for x in 0..dst_w {
            let wi = &weights_h[x as usize];
            let mut sums = [0.0f32; 4];

            unsafe { process_pixel_neon_ps4(src_row, wi, &mut sums, src_w as i32) };

            for c in 0..4 {
                tmp_row[x as usize * 4 + c] = sums[c];
            }
        }
    }

    for y in 0..dst_h {
        let wi = &weights_v[y as usize];
        let out_row = &mut output[(y * dst_w) as usize * 4..];

        for x in 0..dst_w {
            let mut sums = [0.0f32; 4];

            for (k, &w) in wi.weights.iter().enumerate() {
                let sy = (wi.start + k as i32).clamp(0, src_h as i32 - 1);
                let tmp_idx = (sy as usize * dst_w as usize + x as usize) * 4;
                let w_vec = unsafe { vdupq_n_f32(w) };

                let pix = unsafe { vld1q_f32(tmp.as_ptr().add(tmp_idx)) };
                let s = unsafe { vld1q_f32(sums.as_ptr()) };
                let res = unsafe { vfmaq_f32(s, pix, w_vec) };
                unsafe { vst1q_f32(sums.as_mut_ptr(), res) };
            }

            for c in 0..4 {
                out_row[x as usize * 4 + c] = sums[c].clamp(0.0, 255.0) as u8;
            }
        }
    }
}

unsafe fn process_pixel_neon_ps4(src: &[u8], wi: &WeightInfo, sums: &mut [f32; 4], max_w: i32) {
    let mut acc = unsafe { vdupq_n_f32(0.0) };
    for (k, &w) in wi.weights.iter().enumerate() {
        let sx = (wi.start + k as i32).clamp(0, max_w - 1);
        let pix_u8 = unsafe { *(src.as_ptr().add(sx as usize * 4) as *const u32) };

        let v_u8 = unsafe { vset_lane_u32(pix_u8, vdup_n_u32(0), 0) };
        let v_f32 =
            unsafe { vcvtq_f32_u32(vmovl_u16(vget_low_u16(vmovl_u8(vreinterpret_u8_u32(v_u8))))) };

        acc = unsafe { vfmaq_f32(acc, v_f32, vdupq_n_f32(w)) };
    }
    unsafe { vst1q_f32(sums.as_mut_ptr(), acc) };
}

fn resize_fallback(
    input: &[u8],
    output: &mut [u8],
    src_w: u32,
    src_h: u32,
    dst_w: u32,
    dst_h: u32,
    pixel_size: u32,
) {
    let sigma = 1.0;
    let coeffs_h = precompute_gaussian_weights(src_w, dst_w, sigma);
    let coeffs_v = precompute_gaussian_weights(src_h, dst_h, sigma);

    let mut intermediate = vec![0.0f32; (dst_w * src_h * pixel_size) as usize];

    for y in 0..src_h {
        for x in 0..dst_w {
            let weight_info = &coeffs_h[x as usize];
            let out_ptr = ((y * dst_w + x) * pixel_size) as usize;

            for c in 0..pixel_size {
                let mut sum = 0.0f32;
                for i in 0..weight_info.weights.len() {
                    let src_x = (weight_info.start + i as i32).clamp(0, src_w as i32 - 1);
                    let val = input[((y * src_w + src_x as u32) * pixel_size + c) as usize];
                    sum += val as f32 * weight_info.weights[i];
                }
                intermediate[out_ptr + c as usize] = sum;
            }
        }
    }

    for y in 0..dst_h {
        let weight_info = &coeffs_v[y as usize];
        for x in 0..dst_w {
            let out_ptr = ((y * dst_w + x) * pixel_size) as usize;

            for c in 0..pixel_size {
                let mut sum = 0.0f32;
                for i in 0..weight_info.weights.len() {
                    let src_y = (weight_info.start + i as i32).clamp(0, src_h as i32 - 1);
                    let val = intermediate[((src_y as u32 * dst_w + x) * pixel_size + c) as usize];
                    sum += val * weight_info.weights[i];
                }
                output[out_ptr + c as usize] = sum.clamp(0.0, 255.0) as u8;
            }
        }
    }
}

struct WeightInfo {
    start: i32,
    weights: Vec<f32>,
}

fn precompute_gaussian_weights(src_dim: u32, dst_dim: u32, sigma: f32) -> Vec<WeightInfo> {
    let scale = dst_dim as f32 / src_dim as f32;
    let radius = (sigma * 3.0).ceil() as i32;
    let mut all_weights = Vec::with_capacity(dst_dim as usize);

    for i in 0..dst_dim {
        let center = (i as f32 + 0.5) / scale;
        let start = (center.floor() as i32) - radius;
        let mut weights = Vec::new();
        let mut sum = 0.0;

        for j in 0..(radius * 2 + 1) {
            let x = (start + j) as f32 + 0.5;
            let weight = (-((x - center).powi(2)) / (2.0 * sigma * sigma)).exp();
            weights.push(weight);
            sum += weight;
        }

        weights.iter_mut().for_each(|w| *w /= sum);
        all_weights.push(WeightInfo { start, weights });
    }
    all_weights
}
