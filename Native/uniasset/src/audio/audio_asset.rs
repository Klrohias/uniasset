use std::{
    io::{Read, Seek},
    sync::{Arc, RwLock},
};

use crate::native::{NativeHandle, NativeHandleExts};

#[derive(Default)]
pub struct AudioAsset(Box<Arc<RwLock<AudioAssetState>>>);

impl AudioAsset {
    pub fn load_file(&self, path: String) {}
    pub fn load_memory(&self, data: impl AsRef<u8>) {}
    pub fn load_io(&self, stream: impl Read + Seek) {}
}

impl NativeHandleExts for AudioAsset {
    fn into_handle(self) -> NativeHandle {
        self.0.into_handle()
    }

    fn from_handle(handle: NativeHandle) -> Self {
        Self(NativeHandleExts::from_handle(handle))
    }
}

#[derive(Default)]
pub struct AudioAssetState {}
