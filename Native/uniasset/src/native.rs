use std::{error::Error, mem::ManuallyDrop, os::raw::c_void, sync::Arc};

use crate::error::set_error;

pub type NativeHandle = *const c_void;

pub(crate) trait NativeHandleExts {
    fn into_handle(self) -> NativeHandle;
    fn from_handle(handle: NativeHandle) -> Self;
    fn destory(self);
}

impl<T> NativeHandleExts for ManuallyDrop<Box<Arc<T>>> {
    fn into_handle(self) -> NativeHandle {
        Box::into_raw(Self::into_inner(self)) as NativeHandle
    }

    fn from_handle(handle: NativeHandle) -> Self {
        ManuallyDrop::new(unsafe { Box::from_raw(handle as *mut Arc<T>) })
    }

    fn destory(mut self) {
        unsafe { ManuallyDrop::drop(&mut self) }
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
