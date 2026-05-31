use std::{error::Error, ffi::c_int, io, os::raw::c_void, sync::Arc};

use crate::error::set_error;

pub type NativeHandle = *const c_void;

pub(crate) trait NativeHandleExts {
    fn into_handle(self) -> NativeHandle;
    fn from_handle(handle: NativeHandle) -> Self;
}

impl<T> NativeHandleExts for Box<Arc<T>> {
    fn into_handle(self) -> NativeHandle {
        Box::into_raw(self) as NativeHandle
    }

    fn from_handle(handle: NativeHandle) -> Self {
        unsafe { Box::from_raw(handle as *mut Arc<T>) }
    }
}

pub fn failible_to_native<T, E: Error>(
    op: impl FnOnce() -> Result<T, E>,
    default: impl FnOnce() -> T,
) -> T {
    match op() {
        Ok(result) => result,
        Err(err) => {
            set_error(err);
            default()
        }
    }
}
