use std::ffi::c_uint;

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub struct CropOptions {
    pub x: c_uint,
    pub y: c_uint,
    pub width: c_uint,
    pub height: c_uint,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn crop_options_creation() {
        let opts = CropOptions {
            x: 10,
            y: 20,
            width: 100,
            height: 200,
        };
        
        assert_eq!(opts.x, 10);
        assert_eq!(opts.y, 20);
        assert_eq!(opts.width, 100);
        assert_eq!(opts.height, 200);
    }

    #[test]
    fn crop_options_copy() {
        let opts = CropOptions { x: 5, y: 10, width: 50, height: 100 };
        let opts2 = opts;
        
        assert_eq!(opts.x, opts2.x);
        assert_eq!(opts.width, opts2.width);
    }
}
