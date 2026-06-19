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

    pub fn is_empty(&self) -> bool {
        self.len() == 0
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

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn image_buffer_new() {
        let buffer = ImageBuffer::new(100);
        assert_eq!(buffer.len(), 100);
        assert_eq!(buffer.as_ref().len(), 100);
    }

    #[test]
    fn image_buffer_alloc_as_ref() {
        let buffer = ImageBuffer::new(50);
        let slice = buffer.as_ref();
        assert_eq!(slice.len(), 50);
        assert!(slice.iter().all(|&x| x == 0));
    }

    #[test]
    fn image_buffer_alloc_as_mut() {
        let mut buffer = ImageBuffer::new(10);
        let slice = buffer.as_mut();
        slice[0] = 42;
        slice[5] = 99;

        assert_eq!(buffer.as_ref()[0], 42);
        assert_eq!(buffer.as_ref()[5], 99);
    }

    #[test]
    fn image_buffer_clone() {
        let mut buffer = ImageBuffer::new(20);
        buffer.as_mut()[0] = 123;
        buffer.as_mut()[19] = 234;

        let cloned = buffer.clone();
        assert_eq!(cloned.len(), 20);
        assert_eq!(cloned.as_ref()[0], 123);
        assert_eq!(cloned.as_ref()[19], 234);
    }
}
