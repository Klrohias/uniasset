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

type ReadFn = extern "C" fn(user_data: *mut c_void, buf: *mut u8, count: usize) -> isize;
type SeekFn = extern "C" fn(user_data: *mut c_void, offset: i64, whence: c_int) -> i64;

#[repr(C)]
pub struct NativeIOProvider {
    user_data: *mut c_void,
    read_cb: ReadFn,
    seek_cb: SeekFn,
}

impl io::Read for NativeIOProvider {
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        let result = (self.read_cb)(self.user_data, buf.as_mut_ptr(), buf.len());
        if result < 0 {
            return Err(io::Error::new(io::ErrorKind::Other, "External read failed"));
        }
        Ok(result as usize)
    }
}

impl io::Seek for NativeIOProvider {
    fn seek(&mut self, pos: io::SeekFrom) -> io::Result<u64> {
        let (offset, whence) = match pos {
            io::SeekFrom::Start(n) => (n as i64, 0),
            io::SeekFrom::Current(n) => (n, 1),
            io::SeekFrom::End(n) => (n, 2),
        };

        let result = (self.seek_cb)(self.user_data, offset, whence);
        if result < 0 {
            return Err(io::Error::new(io::ErrorKind::Other, "External seek failed"));
        }
        Ok(result as u64)
    }
}
