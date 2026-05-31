use std::{
    os::raw::{c_char, c_uchar},
    ptr::null,
};

use crate::error::{has_error, with_error};

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_HasError() -> c_uchar {
    if has_error() { 1 } else { 0 }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_GetError() -> *const c_char {
    if !has_error() {
        return null();
    }

    with_error(|it| {
        if let Some(error_info) = it {
            error_info.msg.as_ptr()
        } else {
            null()
        }
    })
}
