use crate::{
    audio::AudioAsset,
    error::clear_error,
    native::{NativeHandle, NativeHandleExts},
};

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Create() -> NativeHandle {
    clear_error();
    AudioAsset::default().into_handle()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Destory(handle: NativeHandle) {
    clear_error();
    drop(AudioAsset::from_handle(handle));
}
