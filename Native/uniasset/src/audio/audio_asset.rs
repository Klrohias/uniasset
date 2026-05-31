use std::sync::{Arc, RwLock};

use crate::native::{NativeHandle, NativeHandleExts};

#[derive(Default)]
pub struct AudioAsset(Box<Arc<RwLock<AudioAssetState>>>);

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
