use std::ffi::c_uint;

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub struct CropOptions {
    pub x: c_uint,
    pub y: c_uint,
    pub width: c_uint,
    pub height: c_uint,
}
