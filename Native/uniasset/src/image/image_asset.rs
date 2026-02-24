use std::{
    ffi::c_uchar,
    mem::ManuallyDrop,
    os::raw::c_void,
    path::Path,
    ptr,
    sync::{Arc, RwLock},
};

use stb_image::stb_image::stbi_image_free;

use crate::{
    image::{ImageBuffer, is_jpeg, is_webp},
    native::{NativeHandle, NativeHandleExts},
};

#[derive(Clone, Default)]
pub struct ImageAsset(ManuallyDrop<Box<Arc<RwLock<ImageAssetState>>>>);

impl NativeHandleExts for ImageAsset {
    fn into_handle(self) -> NativeHandle {
        self.0.into_handle()
    }

    fn from_handle(handle: NativeHandle) -> Self {
        Self(NativeHandleExts::from_handle(handle))
    }

    fn destory(self) {
        self.0.destory();
    }
}

impl ImageAsset {
    pub fn load_file(
        &self,
        file_path: impl AsRef<Path>,
        expected_width: i32,
        expected_height: i32,
    ) {
    }

    pub fn load_memory(&self, data: &[u8], expected_width: i32, expected_height: i32) {
        if is_webp(data) {
            // WebP
            todo!()
        }

        if is_jpeg(data) {
            // JPEG
            todo!()
        }

        // BMP, PNG, etc..
        todo!()
    }
}

#[derive(Default)]
struct ImageAssetState {
    buffer: Option<ImageBuffer>,
}
