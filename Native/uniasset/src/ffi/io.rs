use std::{ffi::c_int, io, os::raw::c_void};

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

unsafe impl Send for NativeIOProvider {}
unsafe impl Sync for NativeIOProvider {}
