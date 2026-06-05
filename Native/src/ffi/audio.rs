use std::{
    ffi::{CStr, c_char, c_uchar, c_uint},
    mem::ManuallyDrop,
    slice,
};

use anyhow::anyhow;

use crate::{
    audio::{AudioAsset, SampleFormat},
    ffi::{
        NativeHandle, NativeHandleExts, NativeIOProvider, clear_error, failible_to_native,
        set_error,
    },
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

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_LoadFile(
    handle: NativeHandle,
    path: *const c_char,
    sample_format: c_uchar,
) {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
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

    _ = failible_to_native(|| obj.load_file(path_str.as_ref(), format), || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_LoadMemory(
    handle: NativeHandle,
    data: *const u8,
    size: usize,
    sample_format: c_uchar,
) {
    clear_error();

    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));

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
            // SAFETY: caller guarantees data outlives the audio asset
            let data_ref: &'static [u8] =
                unsafe { std::mem::transmute(slice::from_raw_parts(data, size)) };
            obj.load_memory(data_ref, format)
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

    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));

    let format = match sample_format {
        0 => SampleFormat::Float32,
        1 => SampleFormat::Int16,
        _ => {
            set_error(anyhow!("Invalid sample format: {sample_format}"));
            return;
        }
    };

    _ = failible_to_native(|| obj.load_io(unsafe { &mut *provider }, format), || ());
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Unload(handle: NativeHandle) {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    obj.unload();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetChannelCount(handle: NativeHandle) -> u16 {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    let result = failible_to_native(|| obj.get_channel_count(), || 0);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetSampleCount(handle: NativeHandle) -> u64 {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    let result = failible_to_native(|| obj.get_sample_count(), || 0);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetSampleRate(handle: NativeHandle) -> c_uint {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    let result = failible_to_native(|| obj.get_sample_rate(), || 0);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_GetFrameCount(handle: NativeHandle) -> u64 {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    let result = failible_to_native(|| obj.get_frame_count(), || 0);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Tell(handle: NativeHandle) -> i64 {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    let result = failible_to_native(|| obj.tell(), || 0);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Read(
    handle: NativeHandle,
    buffer: *mut u8,
    buffer_size: usize,
    frame_count: c_uint,
) -> c_uint {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    let result = failible_to_native(
        || {
            let buf = unsafe { slice::from_raw_parts_mut(buffer, buffer_size) };
            obj.read(buf, frame_count)
        },
        || 0,
    );
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_AudioAsset_Seek(handle: NativeHandle, position: i64) {
    clear_error();
    let obj = ManuallyDrop::new(AudioAsset::from_handle(handle));
    _ = failible_to_native(|| obj.seek(position), || ());
}
