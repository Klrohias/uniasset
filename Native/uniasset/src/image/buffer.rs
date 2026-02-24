use std::{
    ffi::{c_uchar, c_void},
    slice,
};

use stb_image::stb_image;

pub enum ImageBuffer {
    Stbi(*mut c_uchar, usize),
    Alloc(Box<[u8]>),
}

impl ImageBuffer {
    pub fn new(size: usize) -> Self {
        Self::Alloc(vec![0u8; size].into_boxed_slice())
    }

    pub fn from_stbi(ptr: *mut c_uchar, size: usize) -> Self {
        Self::Stbi(ptr, size)
    }

    pub fn len(&self) -> usize {
        match self {
            ImageBuffer::Stbi(_, size) => *size,
            ImageBuffer::Alloc(items) => items.len(),
        }
    }
}

impl Clone for ImageBuffer {
    fn clone(&self) -> Self {
        Self::Alloc(self.as_ref().to_vec().into_boxed_slice())
    }
}

impl AsRef<[u8]> for ImageBuffer {
    fn as_ref(&self) -> &[u8] {
        match self {
            ImageBuffer::Stbi(ptr, size) => unsafe { slice::from_raw_parts(*ptr, *size) },
            ImageBuffer::Alloc(items) => items,
        }
    }
}

impl AsMut<[u8]> for ImageBuffer {
    fn as_mut(&mut self) -> &mut [u8] {
        match self {
            ImageBuffer::Stbi(ptr, size) => unsafe { slice::from_raw_parts_mut(*ptr, *size) },
            ImageBuffer::Alloc(items) => items,
        }
    }
}

impl Drop for ImageBuffer {
    fn drop(&mut self) {
        match &self {
            ImageBuffer::Stbi(ptr, _) => unsafe {
                stb_image::stbi_image_free(*ptr as *mut c_void);
            },
            ImageBuffer::Alloc(_) => {}
        }
    }
}
