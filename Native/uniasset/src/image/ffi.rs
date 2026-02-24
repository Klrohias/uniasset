use std::{
    ffi::{c_int, c_uchar, c_uint, c_ulong},
    slice,
};

use anyhow::anyhow;

use crate::{
    error::{clear_error, set_error},
    image::{ImageAsset, ResizeFilter},
    native::{NativeHandle, NativeHandleExts, failible_to_native},
};

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Create() -> NativeHandle {
    clear_error();
    ImageAsset::default().into_handle()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Destory(handle: NativeHandle) {
    clear_error();
    ImageAsset::from_handle(handle).destory();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Load(
    handle: NativeHandle,
    data: *const u8,
    size: c_ulong,
    expected_width: c_uint,
    expected_height: c_uint,
) {
    clear_error();

    let obj = ImageAsset::from_handle(handle);
    failible_to_native(
        || {
            obj.load_memory(
                unsafe { slice::from_raw_parts(data, size as usize) },
                expected_width as u32,
                expected_height as u32,
            )
        },
        || (),
    )
}

// #[unsafe(no_mangle)]
// pub unsafe extern "C" fn Uniasset_ImageAsset_LoadFile(obj_ptr: ImageAssetPtr, path: *const c_char) {
//     clear_error();
//     let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };
//     let path = unsafe { CStr::from_ptr(path) };
//     let path = String::from_utf8_lossy(path.to_bytes());

//     {
//         let mut w = obj.write().unwrap();
//         match w.load_from_file(&path) {
//             Err(e) => {
//                 set_error_for_this_thread(e);
//             }
//             Ok(_) => {}
//         };
//     }

//     forget(obj);
// }

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetWidth(handle: NativeHandle) -> c_uint {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    failible_to_native(|| obj.get_width(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetHeight(handle: NativeHandle) -> c_uint {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    failible_to_native(|| obj.get_height(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_PixelType(handle: NativeHandle) -> c_int {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    failible_to_native(|| obj.get_pixel_type().map(|x| x.into()), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Crop(
    handle: NativeHandle,
    l: c_uint,
    t: c_uint,
    w: c_uint,
    h: c_uint,
) {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    failible_to_native(|| obj.crop(l, t, w, h), || ())
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Resize(
    handle: NativeHandle,
    w: c_uint,
    h: c_uint,
    filter: c_uint,
) {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    let filter = match filter {
        0 => ResizeFilter::Nearest,
        1 => ResizeFilter::Lanczos3,
        2 => ResizeFilter::Gaussian,
        3 => ResizeFilter::Box,
        4_u32..=u32::MAX => {
            set_error(anyhow!("Unsupported filter {}", filter));
            return;
        }
    };
    failible_to_native(|| obj.resize(w, h, filter), || ())
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Unload(handle: NativeHandle) {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    obj.unload();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Clone(handle: NativeHandle) -> NativeHandle {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    obj.deep_clone().into_handle()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_CopyTo(
    handle: NativeHandle,
    dst: *mut c_uchar,
    size: c_ulong,
) {
    clear_error();
    let obj = ImageAsset::from_handle(handle);
    failible_to_native(
        || obj.copy_pixel(unsafe { std::slice::from_raw_parts_mut(dst, size as usize) }),
        || (),
    )
}
