use std::{
    cell::RefCell,
    ffi::CString,
    os::raw::{c_char, c_uchar},
    ptr::null,
};

use anyhow::Error;

thread_local! {
    static ERROR: RefCell<Option<anyhow::Error>> = RefCell::new(None);
    static ERROR_MSG: RefCell<Option<CString>> = RefCell::new(None);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_HasError() -> c_uchar {
    ERROR.with_borrow(|x| if x.is_none() { 0 } else { 1 })
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_GetError() -> *const c_char {
    if ERROR.with_borrow(|x| x.is_none()) {
        return null();
    }

    ERROR_MSG.with_borrow_mut(|x| {
        if x.is_none() {
            if let Ok(y) = CString::new(ERROR.with_borrow(|y| y.as_ref().unwrap().to_string())) {
                *x = Some(y);
            }
        }
    });

    ERROR_MSG.with_borrow(|x| x.as_ref().unwrap().as_ptr())
}

pub fn clear_error_for_this_thread() {
    ERROR.replace(None);
    ERROR_MSG.replace(None);
}

pub fn set_error_for_this_thread(err: Error) {
    ERROR_MSG.replace(None);
    ERROR.replace(Some(err));
}
