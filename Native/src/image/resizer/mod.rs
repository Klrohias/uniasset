mod nearest;
pub use nearest::*;
mod box_filter;
pub use box_filter::*;
mod gaussian;
pub use gaussian::*;
mod lanczos3;
pub use lanczos3::*;

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum ResizeFilter {
    Nearest,
    Box,
    Lanczos3,
    Gaussian,
}

pub fn resize(
    input: &[u8],
    output: &mut [u8],
    origin_width: u32,
    origin_height: u32,
    dest_width: u32,
    dest_height: u32,
    pixel_size: u32,
    filter: ResizeFilter,
) {
    match filter {
        ResizeFilter::Nearest => nearest_resize_dispatch(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
            pixel_size,
        ),
        ResizeFilter::Box => box_resize_dispatch(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
            pixel_size,
        ),
        ResizeFilter::Lanczos3 => lanczos3_dispatch(
            input,
            output,
            origin_width,
            origin_height,
            dest_width,
            dest_height,
            pixel_size,
        ),
        ResizeFilter::Gaussian => gaussian_dispatch(
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
