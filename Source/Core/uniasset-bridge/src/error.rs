use std::{
    os::raw::{c_char, c_uchar},
    ptr::null,
};

use std::{cell::RefCell, ffi::CString, fmt::Display};

thread_local! {
    static ERROR_INFO: RefCell<Option<ErrorInfo>> = RefCell::new(None);
}

#[derive(Clone)]
pub struct ErrorInfo {
    pub msg: CString,
}

pub fn has_error() -> bool {
    ERROR_INFO.with_borrow(|it| it.is_some())
}

pub fn with_error<T>(f: impl FnOnce(&Option<ErrorInfo>) -> T) -> T {
    ERROR_INFO.with_borrow(|it| f(it))
}

pub fn set_error(error: impl Display) {
    let message = CString::new(format!("{error}")).unwrap();
    ERROR_INFO.replace(Some(ErrorInfo { msg: message }));
}

pub fn clear_error() {
    ERROR_INFO.set(None);
}

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
