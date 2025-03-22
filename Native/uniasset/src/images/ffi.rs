use crate::ffi::{clear_error_for_this_thread, set_error_for_this_thread};
use std::{
    ffi::CStr,
    mem::forget,
    os::raw::{c_char, c_uchar, c_uint, c_ulong},
    sync::{Arc, RwLock},
};

use super::ImageAsset;

type ImageAssetPtr = *const RwLock<ImageAsset>;
type ImageAssetFFI = Arc<RwLock<ImageAsset>>;

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Create() -> ImageAssetPtr {
    clear_error_for_this_thread();
    ImageAssetFFI::into_raw(ImageAssetFFI::new(RwLock::new(ImageAsset::new())))
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Destory(obj_ptr: ImageAssetPtr) {
    clear_error_for_this_thread();
    unsafe { drop(ImageAssetFFI::from_raw(obj_ptr)) }
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Load(
    obj_ptr: ImageAssetPtr,
    data: *const u8,
    size: c_ulong,
) {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    {
        let mut w = obj.write().unwrap();
        match w.load_from_memory(unsafe { std::slice::from_raw_parts(data, size as usize) }) {
            Err(e) => {
                set_error_for_this_thread(e);
            }
            Ok(_) => {}
        };
    }

    forget(obj);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_LoadFile(obj_ptr: ImageAssetPtr, path: *const c_char) {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };
    let path = unsafe { CStr::from_ptr(path) };
    let path = String::from_utf8_lossy(path.to_bytes());

    {
        let mut w = obj.write().unwrap();
        match w.load_from_file(&path) {
            Err(e) => {
                set_error_for_this_thread(e);
            }
            Ok(_) => {}
        };
    }

    forget(obj);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetWidth(obj_ptr: ImageAssetPtr) -> c_uint {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    let result = {
        let r = obj.read().unwrap();
        match r.get_width() {
            Err(e) => {
                set_error_for_this_thread(e);
                0
            }
            Ok(v) => v,
        }
    };

    forget(obj);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_GetHeight(obj_ptr: ImageAssetPtr) -> c_uint {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    let result = {
        let r = obj.read().unwrap();
        match r.get_height() {
            Err(e) => {
                set_error_for_this_thread(e);
                0
            }
            Ok(v) => v,
        }
    };

    forget(obj);
    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Crop(
    obj_ptr: ImageAssetPtr,
    l: c_uint,
    t: c_uint,
    w: c_uint,
    h: c_uint,
) {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    {
        let mut o = obj.write().unwrap();
        match o.crop(l, t, w, h) {
            Err(e) => {
                set_error_for_this_thread(e);
            }
            Ok(_) => {}
        };
    }

    forget(obj);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Resize(obj_ptr: ImageAssetPtr, w: c_uint, h: c_uint) {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    {
        let mut o = obj.write().unwrap();
        match o.resize(w, h) {
            Err(e) => {
                set_error_for_this_thread(e);
            }
            Ok(_) => {}
        };
    }

    forget(obj);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Unload(obj_ptr: ImageAssetPtr) {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    {
        let mut o = obj.write().unwrap();
        o.unload();
    }

    forget(obj);
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_Clone(obj_ptr: ImageAssetPtr) -> ImageAssetPtr {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    let result = {
        let o = obj.read().unwrap();
        ImageAssetFFI::into_raw(ImageAssetFFI::new(RwLock::new(o.clone())))
    };

    forget(obj);

    result
}

#[unsafe(no_mangle)]
pub unsafe extern "C" fn Uniasset_ImageAsset_CopyTo(
    obj_ptr: ImageAssetPtr,
    dst: *mut c_uchar,
    size: c_ulong,
) {
    clear_error_for_this_thread();
    let obj = unsafe { ImageAssetFFI::from_raw(obj_ptr) };

    {
        let o = obj.read().unwrap();
        match o.copy_to(unsafe { std::slice::from_raw_parts_mut(dst, size as usize) }) {
            Err(e) => {
                set_error_for_this_thread(e);
            }
            Ok(_) => {}
        }
    };

    forget(obj);
}
