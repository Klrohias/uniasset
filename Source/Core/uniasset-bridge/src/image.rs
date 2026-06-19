use std::{
    ffi::{CStr, c_char, c_uchar, c_uint, c_ulong},
    mem::ManuallyDrop,
    path::PathBuf,
    slice,
    sync::Arc,
};

use anyhow::anyhow;
use parking_lot::RwLock;

use crate::{
    error::{clear_error, set_error},
    io::NativeIOProvider,
    object::{NativeHandle, NativeHandleExts, failible_to_native},
};
use uniasset::image::{CropOptions, ImageAsset, resizer::ResizeFilter};

pub type ImageAssetWrapper = Box<Arc<RwLock<ImageAsset>>>;

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Create() -> NativeHandle {
    clear_error();
    Box::new(Arc::new(RwLock::new(ImageAsset::default()))).into_handle()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Destory(handle: NativeHandle) {
    clear_error();
    drop(ImageAssetWrapper::from_handle(handle));
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
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    let result = failible_to_native(
        || {
            wrapper.write().load_memory(
                unsafe { slice::from_raw_parts(data, size as usize) },
                expected_width as u32,
                expected_height as u32,
            )
        },
        || (),
    );
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_LoadFile(
    handle: NativeHandle,
    path: *const c_char,
    expected_width: c_uint,
    expected_height: c_uint,
) {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    let path_slice = unsafe { CStr::from_ptr(path) };
    let path_str = path_slice.to_string_lossy();
    let path_buf = PathBuf::from(path_str.as_ref());

    _ = failible_to_native(
        || {
            wrapper
                .write()
                .load_file(path_buf, expected_width, expected_height)
        },
        || (),
    );
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_LoadIO(
    handle: NativeHandle,
    provider: *mut NativeIOProvider,
    expected_width: c_uint,
    expected_height: c_uint,
) {
    clear_error();

    if provider.is_null() {
        set_error(anyhow!("Invaild IO provider"));
        return;
    }

    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    _ = failible_to_native(
        || {
            wrapper.write().load_io(
                unsafe { &mut *provider },
                expected_width as u32,
                expected_height as u32,
            )
        },
        || (),
    );
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetWidth(handle: NativeHandle) -> c_uint {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.read().get_width(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetHeight(handle: NativeHandle) -> c_uint {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.read().get_height(), || 0)
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetPixelType(handle: NativeHandle) -> c_uint {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.read().get_pixel_type().map(|x| x as _), || 0)
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
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    failible_to_native(|| wrapper.write().crop(l, t, w, h), || ())
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Resize(
    handle: NativeHandle,
    w: c_uint,
    h: c_uint,
    filter: c_uint,
) {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    let filter = match filter {
        0 => ResizeFilter::Nearest,
        1 => ResizeFilter::Box,
        2 => ResizeFilter::Lanczos3,
        3 => ResizeFilter::Gaussian,
        _ => {
            set_error(anyhow!("Unsupported filter {}", filter));
            return;
        }
    };
    failible_to_native(|| wrapper.write().resize(w, h, filter), || ())
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Unload(handle: NativeHandle) {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    wrapper.write().unload();
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Clone(handle: NativeHandle) -> NativeHandle {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    let cloned = wrapper.read().clone();
    Box::new(Arc::new(RwLock::new(cloned))).into_handle()
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_CopyTo(
    handle: NativeHandle,
    dst: *mut c_uchar,
    size: c_ulong,
) {
    clear_error();
    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));
    _ = failible_to_native(
        || {
            wrapper
                .read()
                .copy_pixel(unsafe { std::slice::from_raw_parts_mut(dst, size as usize) })
        },
        || (),
    );
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_CropMultiple(
    handle: NativeHandle,
    items: *const CropOptions,
    count: c_uint,
    output: *mut NativeHandle,
) {
    clear_error();

    if items.is_null() || output.is_null() {
        set_error(anyhow!("Invaild crop options/output"));
        return;
    }

    let wrapper = ManuallyDrop::new(ImageAssetWrapper::from_handle(handle));

    _ = failible_to_native(
        || {
            let items = unsafe { slice::from_raw_parts(items, count as usize) };
            let output = unsafe { slice::from_raw_parts_mut(output, count as usize) };

            let results = wrapper.read().crop_multiple(items)?;
            for (i, asset) in results.into_iter().enumerate() {
                output[i] = Box::new(Arc::new(RwLock::new(asset))).into_handle();
            }

            Ok::<(), uniasset::image::ImageOperationError>(())
        },
        || (),
    );
}
