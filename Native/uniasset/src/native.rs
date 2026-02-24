use std::{mem::ManuallyDrop, os::raw::c_void, sync::Arc};

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
