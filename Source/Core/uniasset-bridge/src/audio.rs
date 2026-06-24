use std::{
    ffi::{CStr, c_char, c_uchar, c_uint},
    mem::ManuallyDrop,
    slice,
    sync::Arc,
};

use anyhow::anyhow;

use crate::{
    error::{clear_error, set_error},
    io::NativeIOProvider,
    object::{NativeHandle, NativeHandleExts, failible_to_native},
};
use uniasset::audio::{AudioAsset, SampleFormat};

pub type AudioAssetWrapper = Box<Arc<AudioAsset>>;

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Create() -> NativeHandle {
    clear_error();
    Box::new(Arc::new(AudioAsset::default())).into_handle()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Destory(handle: NativeHandle) {
    clear_error();
    drop(AudioAssetWrapper::from_handle(handle));
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_LoadFile(
    handle: NativeHandle,
    path: *const c_char,
    sample_format: c_uchar,
) {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    let path_slice = unsafe { CStr::from_ptr(path) };
    let path_str = path_slice.to_string_lossy();

    let format = match sample_format {
        0 => SampleFormat::Float32,
        1 => SampleFormat::Int16,
        _ => {
            set_error(anyhow!("Invalid sample format: {sample_format}"));
            return;
        }
    };

    _ = failible_to_native(|| wrapper.load_file(path_str.as_ref(), format), || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_LoadMemory(
    handle: NativeHandle,
    data: *const u8,
    size: usize,
    sample_format: c_uchar,
) {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));

    let format = match sample_format {
        0 => SampleFormat::Float32,
        1 => SampleFormat::Int16,
        _ => {
            set_error(anyhow!("Invalid sample format: {sample_format}"));
            return;
        }
    };

    _ = failible_to_native(
        || {
            let data_ref: &'static [u8] =
                unsafe { std::mem::transmute(slice::from_raw_parts(data, size)) };
            wrapper.load_memory(data_ref, format)
        },
        || (),
    );
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_LoadIO(
    handle: NativeHandle,
    provider: *mut NativeIOProvider,
    sample_format: c_uchar,
) {
    clear_error();

    if provider.is_null() {
        set_error(anyhow!("Invaild IO provider"));
        return;
    }

    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));

    let format = match sample_format {
        0 => SampleFormat::Float32,
        1 => SampleFormat::Int16,
        _ => {
            set_error(anyhow!("Invalid sample format: {sample_format}"));
            return;
        }
    };

    _ = failible_to_native(|| wrapper.load_io(unsafe { &mut *provider }, format), || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Unload(handle: NativeHandle) {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    wrapper.unload();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetChannelCount(handle: NativeHandle) -> u16 {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.get_channel_count(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetSampleCount(handle: NativeHandle) -> u64 {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.get_sample_count(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetSampleRate(handle: NativeHandle) -> c_uint {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.get_sample_rate(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetSampleFormat(handle: NativeHandle) -> u8 {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.get_sample_format().map(|it| it as u8), || 0u8)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetFrameCount(handle: NativeHandle) -> u64 {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.get_frame_count(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Tell(handle: NativeHandle) -> i64 {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.tell(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Read(
    handle: NativeHandle,
    buffer: *mut u8,
    buffer_size: usize,
    frame_count: c_uint,
) -> c_uint {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(
        || {
            let buf = unsafe { slice::from_raw_parts_mut(buffer, buffer_size) };
            wrapper.read(buf, frame_count)
        },
        || 0,
    )
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Seek(handle: NativeHandle, position: i64) {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    _ = failible_to_native(|| wrapper.seek(position), || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_ReadUnsafe(
    handle: NativeHandle,
    buffer: *mut u8,
    buffer_size: usize,
    frame_count: c_uint,
) -> c_uint {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(
        || {
            let buf = unsafe { slice::from_raw_parts_mut(buffer, buffer_size) };
            unsafe { wrapper.read_unsafe(buf, frame_count) }
        },
        || 0,
    )
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_SeekUnsafe(handle: NativeHandle, position: i64) {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    _ = failible_to_native(|| unsafe { wrapper.seek_unsafe(position) }, || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Prepare(handle: NativeHandle) {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    _ = failible_to_native(|| wrapper.prepare(), || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_TryClone(handle: NativeHandle) -> NativeHandle {
    clear_error();
    let wrapper = ManuallyDrop::new(AudioAssetWrapper::from_handle(handle));
    failible_to_native(
        || {
            wrapper
                .try_clone()
                .map(|cloned| Box::new(Arc::new(cloned)).into_handle())
        },
        || std::ptr::null(),
    )
}
