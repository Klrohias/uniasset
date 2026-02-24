use std::{
    error::Error,
    ffi::{c_char, c_int, c_uchar, c_void},
    fmt::Display,
    fs::File,
    io::{self, BufReader, Read, Seek, SeekFrom},
    mem::{ManuallyDrop, MaybeUninit},
    path::Path,
    ptr::null_mut,
    slice,
    sync::{Arc, RwLock},
};

use fast_image_resize as fr;
use libwebp_sys::{
    VP8StatusCode, WEBP_CSP_MODE, WebPBitstreamFeatures, WebPDecode, WebPDecoderConfig,
    WebPFreeDecBuffer, WebPGetInfo,
};
use stb_image::stb_image;
use zune_jpeg::{JpegDecoder, zune_core::bytestream::ZCursor};

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
        expected_width: u32,
        expected_height: u32,
    ) -> Result<(), ImageOperationError> {
        self.load_io(File::open(file_path)?, expected_width, expected_height)
    }

    pub fn load_io(
        &self,
        mut stream: impl Read + Seek,
        expected_width: u32,
        expected_height: u32,
    ) -> Result<(), ImageOperationError> {
        let mut magic_number_buffer = [0u8; 16];
        stream.read_exact(&mut magic_number_buffer)?;
        stream.seek(io::SeekFrom::Start(0))?;

        // Check magic number and decode
        let (buffer, info) = if is_webp(&magic_number_buffer) {
            // WebP
            todo!()
        } else if is_jpeg(&magic_number_buffer) {
            // JPEG
            Self::load_jpeg_io(stream)?
        } else {
            // Any STBI supported
            Self::load_stbi_io(stream)?
        };

        let mut state = self.0.write().unwrap();
        state.buffer = Some(buffer);
        state.info = Some(info.clone());

        // Maybe someone wants write below, drop the lock here.
        drop(state);

        // Specified size is expected, but the current image data doesn't match
        if expected_height != 0
            && expected_width != 0
            && expected_height != info.height
            && expected_width != info.width
        {
            self.resize(expected_width, expected_height, ResizeFilter::Lanczos3)?;
        }

        Ok(())
    }

    fn load_webp_io(
        data: &[u8],
        expected_width: u32,
        expected_height: u32,
    ) -> Result<(ImageBuffer, ImageInfo), ImageOperationError> {
        todo!()
    }

    fn load_jpeg_io(
        stream: impl Read + Seek,
    ) -> Result<(ImageBuffer, ImageInfo), ImageOperationError> {
        let mut decoder = JpegDecoder::new(BufReader::new(stream));
        decoder.decode_headers()?;
        let (output_width, output_height) = decoder.dimensions().unwrap();
        let (output_width, output_height) = (output_width as u32, output_height as u32);

        let target_pixel_size = PixelType::RGB; // Jpeg should be RGB
        let mut buffer = ImageBuffer::new(
            (output_width * output_height) as usize * target_pixel_size.get_size(),
        );
        decoder.decode_into(buffer.as_mut())?;

        // Fill image info
        let info = ImageInfo {
            width: output_width,
            height: output_height,
            pixel_type: target_pixel_size,
        };

        Ok((buffer, info))
    }

    fn load_stbi_io<S: Read + Seek>(
        stream: S,
    ) -> Result<(ImageBuffer, ImageInfo), ImageOperationError> {
        let mut wrapper = StbIoWrapper { reader: stream };
        let callbacks = stb_image::stbi_io_callbacks {
            read: Some(stb_read_callback::<S>),
            skip: Some(stb_skip_callback::<S>),
            eof: Some(stb_eof_callback::<S>),
        };

        let mut output_width: c_int = 0;
        let mut output_height: c_int = 0;
        let mut output_channels: c_int = 0;

        let data = unsafe {
            stb_image::stbi_load_from_callbacks(
                &callbacks as *const _,
                &mut wrapper as *mut _ as *mut _,
                &mut output_width as *mut _,
                &mut output_height as *mut _,
                &mut output_channels as *mut _,
                0,
            )
        };

        if data == null_mut() {
            return Err(ImageOperationError::UnsupportedImage);
        }

        let pixel_type = if output_channels == 1 {
            PixelType::Grey
        } else if output_channels == 3 {
            PixelType::RGB
        } else if output_channels == 4 {
            PixelType::RGBA
        } else {
            // Don't remember to free
            unsafe {
                stb_image::stbi_image_free(data as *mut c_void);
            }

            return Err(ImageOperationError::UnsupportPixelType);
        };

        let buffer = ImageBuffer::from_stbi(
            data,
            (output_height * output_height) as usize * pixel_type.get_size(),
        );

        let info = ImageInfo {
            width: output_width as u32,
            height: output_height as u32,
            pixel_type,
        };

        Ok((buffer, info))
    }

    pub fn load_memory(
        &self,
        data: &[u8],
        expected_width: u32,
        expected_height: u32,
    ) -> Result<(), ImageOperationError> {
        let (buffer, info) = if is_webp(data) {
            // WebP
            Self::load_webp_memory(data, expected_width, expected_height)?
        } else if is_jpeg(data) {
            // JPEG
            Self::load_jpeg_memory(data)?
        } else {
            // Any STBI supported
            Self::load_stbi_memory(data)?
        };

        let mut state = self.0.write().unwrap();
        state.buffer = Some(buffer);
        state.info = Some(info.clone());

        // Maybe someone wants write below, drop the lock here.
        drop(state);

        // Specified size is expected, but the current image data doesn't match
        if expected_height != 0
            && expected_width != 0
            && expected_height != info.height
            && expected_width != info.width
        {
            self.resize(expected_width, expected_height, ResizeFilter::Lanczos3)?;
        }

        Ok(())
    }

    fn load_jpeg_memory(data: &[u8]) -> Result<(ImageBuffer, ImageInfo), ImageOperationError> {
        // So tidy code, thanks to zune-image team!!

        let mut decoder = JpegDecoder::new(ZCursor::new(data));
        decoder.decode_headers()?;
        let (output_width, output_height) = decoder.dimensions().unwrap();
        let (output_width, output_height) = (output_width as u32, output_height as u32);

        let target_pixel_size = PixelType::RGB; // Jpeg should be RGB
        let mut buffer = ImageBuffer::new(
            (output_width * output_height) as usize * target_pixel_size.get_size(),
        );
        decoder.decode_into(buffer.as_mut())?;

        // Fill image info
        let info = ImageInfo {
            width: output_width,
            height: output_height,
            pixel_type: target_pixel_size,
        };

        Ok((buffer, info))
    }

    fn load_webp_memory(
        data: &[u8],
        expected_width: u32,
        expected_height: u32,
    ) -> Result<(ImageBuffer, ImageInfo), ImageOperationError> {
        let mut original_width: i32 = 0;
        let mut original_height: i32 = 0;

        // Get info, ensure the given data is WebP.
        if unsafe {
            WebPGetInfo(
                data.as_ptr(),
                data.len(),
                &mut original_width as *mut i32,
                &mut original_height as *mut i32,
            )
        } == 0
        {
            return Err(ImageOperationError::UnsupportedImage);
        }

        // Get features, check if the image have alpha channel
        let mut features = MaybeUninit::<WebPBitstreamFeatures>::uninit();
        let mut status_code = unsafe {
            libwebp_sys::WebPGetFeatures(data.as_ptr(), data.len(), features.as_mut_ptr())
        };
        if status_code != VP8StatusCode::VP8_STATUS_OK {
            return Err(ImageOperationError::WebPError(status_code));
        }

        let features = unsafe { features.assume_init() };
        let has_alpha_channel = features.has_alpha != 0;
        let target_channel_count = if has_alpha_channel { 4 } else { 3 };
        let use_scaling = expected_width > 0 && expected_height > 0;

        let target_width = if use_scaling {
            expected_width
        } else {
            original_width as u32
        };

        let target_height = if use_scaling {
            expected_height
        } else {
            original_height as u32
        };

        let stride_size = target_channel_count * target_width;

        // Decode it
        // Allocate buffer
        let mut buffer =
            ImageBuffer::new((target_width * target_height * target_channel_count) as usize);

        // Fill decoder config
        let mut decoder_config = WebPDecoderConfig::new()
            .map_err(|_| ImageOperationError::WebPDecoderInitializationError)?;

        decoder_config.options.use_threads = 1;
        if use_scaling {
            decoder_config.options.use_scaling = 1;
            decoder_config.options.scaled_width = expected_width as i32;
            decoder_config.options.scaled_height = expected_height as i32;
        }
        decoder_config.options.use_scaling = if use_scaling { 1 } else { 0 };

        decoder_config.input = features;

        decoder_config.output.is_external_memory = 1;
        decoder_config.output.colorspace = if has_alpha_channel {
            WEBP_CSP_MODE::MODE_RGBA
        } else {
            WEBP_CSP_MODE::MODE_RGB
        };
        decoder_config.output.u.RGBA.stride = -(stride_size as i32);
        decoder_config.output.u.RGBA.rgba = buffer
            .as_mut()
            .as_mut_ptr()
            .wrapping_add(((target_height - 1) * stride_size) as usize);
        decoder_config.output.u.RGBA.size = (target_height * stride_size) as usize;
        decoder_config.output.width = target_width as i32;
        decoder_config.output.height = target_height as i32;

        // Invoke decoder
        status_code =
            unsafe { WebPDecode(data.as_ptr(), data.len(), &mut decoder_config as *mut _) };

        unsafe {
            WebPFreeDecBuffer(&mut decoder_config.output as *mut _);
        };

        if status_code != VP8StatusCode::VP8_STATUS_OK {
            return Err(ImageOperationError::WebPError(status_code));
        }

        // Fill image info
        let info = ImageInfo {
            width: target_width,
            height: target_height,
            pixel_type: if has_alpha_channel {
                PixelType::RGBA
            } else {
                PixelType::RGB
            },
        };

        Ok((buffer, info))
    }

    fn load_stbi_memory(data: &[u8]) -> Result<(ImageBuffer, ImageInfo), ImageOperationError> {
        unsafe {
            stb_image::stbi_set_flip_vertically_on_load_thread(1);
        }

        let mut output_width: c_int = 0;
        let mut output_height: c_int = 0;
        let mut output_channels: c_int = 0;

        let data = unsafe {
            stb_image::stbi_load_from_memory(
                data.as_ptr(),
                data.len() as i32,
                &mut output_width as *mut _,
                &mut output_height as *mut _,
                &mut output_channels as *mut _,
                0,
            )
        };

        if data == null_mut() {
            return Err(ImageOperationError::UnsupportedImage);
        }

        let pixel_type = if output_channels == 1 {
            PixelType::Grey
        } else if output_channels == 3 {
            PixelType::RGB
        } else if output_channels == 4 {
            PixelType::RGBA
        } else {
            // Don't remember to free
            unsafe {
                stb_image::stbi_image_free(data as *mut c_void);
            }

            return Err(ImageOperationError::UnsupportPixelType);
        };

        let buffer = ImageBuffer::from_stbi(
            data,
            (output_height * output_height) as usize * pixel_type.get_size(),
        );

        let info = ImageInfo {
            width: output_width as u32,
            height: output_height as u32,
            pixel_type,
        };

        Ok((buffer, info))
    }

    pub fn get_width(&self) -> Result<u32, ImageOperationError> {
        let state = self.0.read().unwrap();

        match state.info.as_ref() {
            Some(info) => Ok(info.width),
            None => Err(ImageOperationError::Unavailable),
        }
    }

    pub fn get_height(&self) -> Result<u32, ImageOperationError> {
        let state = self.0.read().unwrap();

        match state.info.as_ref() {
            Some(info) => Ok(info.height),
            None => Err(ImageOperationError::Unavailable),
        }
    }

    pub fn get_pixel_type(&self) -> Result<PixelType, ImageOperationError> {
        let state = self.0.read().unwrap();

        match state.info.as_ref() {
            Some(info) => Ok(info.pixel_type),
            None => Err(ImageOperationError::Unavailable),
        }
    }

    pub fn unload(&self) {
        let mut state = self.0.write().unwrap();
        state.buffer = None;
        state.info = None;
    }

    pub fn crop(&self, x: u32, y: u32, width: u32, height: u32) -> Result<(), ImageOperationError> {
        let mut state = self.0.write().unwrap();

        if state.info.is_none() || state.buffer.is_none() {
            return Err(ImageOperationError::Unavailable);
        }

        // Check the crop range
        let old_info = state.info.take().unwrap();
        if x > old_info.width
            || x + width > old_info.width
            || y > old_info.height
            || y + height > old_info.height
        {
            return Err(ImageOperationError::Overflow);
        }

        // Crop
        let top_y = old_info.height - (y + height);

        let pixel_size = old_info.pixel_type.get_size() as u32;
        let src_stride = old_info.width * pixel_size;
        let dst_stride = width * pixel_size;

        let old_buffer = state.buffer.take().unwrap();
        let mut new_buffer =
            ImageBuffer::new((width * height) as usize * old_info.pixel_type.get_size());

        let src_data = old_buffer.as_ref();
        let dst_data = new_buffer.as_mut();

        for row in 0..height {
            let src_row_start = ((top_y + row) * src_stride + x * pixel_size) as usize;
            let src_row_end = src_row_start + dst_stride as usize;

            let dst_row_start = (row * dst_stride) as usize;
            let dst_row_end = dst_row_start + dst_stride as usize;

            dst_data[dst_row_start..dst_row_end]
                .copy_from_slice(&src_data[src_row_start..src_row_end]);
        }

        // Fill new info
        let mut new_info = old_info.clone();
        new_info.width = width;
        new_info.height = height;

        state.info = Some(new_info);
        state.buffer = Some(new_buffer);
        Ok(())
    }

    pub fn resize(
        &self,
        new_width: u32,
        new_height: u32,
        filter_type: ResizeFilter,
    ) -> Result<(), ImageOperationError> {
        let mut state = self.0.write().unwrap();
        if state.info.is_none() || state.buffer.is_none() {
            return Err(ImageOperationError::Unavailable);
        }

        let old_info = state.info.take().unwrap();
        let mut old_buffer = state.buffer.take().unwrap();
        let fr_pixel_type = old_info.pixel_type.get_resizer_pixel_type();

        // Resize
        let mut new_buffer =
            ImageBuffer::new((new_width * new_height) as usize * old_info.pixel_type.get_size());
        let old_image = fr::images::Image::from_slice_u8(
            old_info.width,
            old_info.height,
            old_buffer.as_mut(),
            fr_pixel_type,
        )?;
        let mut new_image = fr::images::Image::from_slice_u8(
            new_width,
            new_height,
            new_buffer.as_mut(),
            fr_pixel_type,
        )?;
        let mut resizer = fr::Resizer::new();
        resizer.resize(
            &old_image,
            &mut new_image,
            &fr::ResizeOptions {
                algorithm: match filter_type {
                    ResizeFilter::Nearest => fr::ResizeAlg::Nearest,
                    ResizeFilter::Box => fr::ResizeAlg::Convolution(fr::FilterType::Box),
                    ResizeFilter::Lanczos3 => fr::ResizeAlg::Convolution(fr::FilterType::Lanczos3),
                    ResizeFilter::Gaussian => fr::ResizeAlg::Convolution(fr::FilterType::Gaussian),
                },
                ..Default::default()
            },
        )?;

        // Fill new info
        let mut new_info = old_info.clone();
        new_info.width = new_width;
        new_info.height = new_height;

        state.info = Some(new_info);
        state.buffer = Some(new_buffer);
        Ok(())
    }

    pub fn deep_clone(&self) -> ImageAsset {
        let new_instance = ImageAsset::default();
        let state = self.0.read().unwrap();
        {
            let mut new_state = new_instance.0.write().unwrap();

            new_state.buffer = state.buffer.clone();
            new_state.info = state.info.clone();
        }
        new_instance
    }

    pub fn copy_pixel(&self, target: &mut [u8]) -> Result<(), ImageOperationError> {
        let state = self.0.read().unwrap();
        if let Some(buffer) = state.buffer.as_ref() {
            target.copy_from_slice(buffer.as_ref());
            Ok(())
        } else {
            Err(ImageOperationError::Unavailable)
        }
    }
}

#[derive(PartialEq, Eq, Clone, Copy)]
pub enum PixelType {
    RGBA = 1,
    ARGB = 2,
    RGB = 3,
    Grey = 4,
}

impl PixelType {
    pub fn get_size(&self) -> usize {
        match self {
            PixelType::RGBA => 4,
            PixelType::ARGB => 4,
            PixelType::RGB => 3,
            PixelType::Grey => 1,
        }
    }

    fn get_resizer_pixel_type(&self) -> fr::PixelType {
        match self {
            PixelType::RGBA => fr::PixelType::U8x4,
            PixelType::ARGB => fr::PixelType::U8x4,
            PixelType::RGB => fr::PixelType::U8x3,
            PixelType::Grey => fr::PixelType::U8,
        }
    }
}

impl Into<i32> for PixelType {
    fn into(self) -> i32 {
        self as i32
    }
}

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum ResizeFilter {
    Nearest,
    Box,
    Lanczos3,
    Gaussian,
}

#[derive(Clone)]
struct ImageInfo {
    pixel_type: PixelType,
    width: u32,
    height: u32,
}

#[derive(Default)]
struct ImageAssetState {
    buffer: Option<ImageBuffer>,
    info: Option<ImageInfo>,
}

#[derive(Debug)]
pub enum ImageOperationError {
    Unavailable,
    Overflow,
    ImageBufferError(fr::ImageBufferError),
    ResizeError(fr::ResizeError),
    UnsupportPixelType,
    UnsupportedImage,
    WebPError(VP8StatusCode),
    WebPDecoderInitializationError,
    JpegDecodeError(zune_jpeg::errors::DecodeErrors),
    IOError(io::Error),
}

impl Display for ImageOperationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ImageOperationError::Unavailable => write!(f, "No image asset is available"),
            ImageOperationError::ImageBufferError(e) => write!(f, "ImageBufferError: {}", e),
            ImageOperationError::ResizeError(e) => write!(f, "ResizeError: {}", e),
            ImageOperationError::Overflow => write!(f, "Out of the origin image resolution"),
            ImageOperationError::UnsupportPixelType => write!(f, "The pixel type is not supported"),
            ImageOperationError::UnsupportedImage => {
                write!(f, "The kind of image is not supported")
            }
            ImageOperationError::WebPError(e) => write!(f, "WebP Error: {e:?}"),
            ImageOperationError::WebPDecoderInitializationError => {
                write!(f, "Failed to initialize WebP Decoder")
            }
            ImageOperationError::JpegDecodeError(e) => write!(f, "Jpeg Error: {e}"),
            ImageOperationError::IOError(e) => write!(f, "IO Error: {e}"),
        }
    }
}

impl Error for ImageOperationError {}

impl From<fr::ImageBufferError> for ImageOperationError {
    fn from(value: fr::ImageBufferError) -> Self {
        Self::ImageBufferError(value)
    }
}

impl From<fr::ResizeError> for ImageOperationError {
    fn from(value: fr::ResizeError) -> Self {
        Self::ResizeError(value)
    }
}

impl From<zune_jpeg::errors::DecodeErrors> for ImageOperationError {
    fn from(value: zune_jpeg::errors::DecodeErrors) -> Self {
        Self::JpegDecodeError(value)
    }
}

impl From<io::Error> for ImageOperationError {
    fn from(value: io::Error) -> Self {
        Self::IOError(value)
    }
}

struct StbIoWrapper<R: Read + Seek> {
    reader: R,
}

unsafe extern "C" fn stb_read_callback<R: Read + Seek>(
    user: *mut c_void,
    data: *mut c_char,
    size: c_int,
) -> i32 {
    let wrapper = unsafe { &mut *(user as *mut StbIoWrapper<R>) };
    let slice = unsafe { slice::from_raw_parts_mut(data as *mut c_uchar, size as usize) };
    wrapper.reader.read(slice).unwrap_or(0) as i32
}

unsafe extern "C" fn stb_skip_callback<R: Read + Seek>(user: *mut c_void, n: i32) {
    let wrapper = unsafe { &mut *(user as *mut StbIoWrapper<R>) };
    let _ = wrapper.reader.seek(SeekFrom::Current(n as i64));
}

unsafe extern "C" fn stb_eof_callback<R: Read + Seek>(user: *mut c_void) -> i32 {
    let wrapper = unsafe { &mut *(user as *mut StbIoWrapper<R>) };
    let mut buf = [0u8; 1];
    match wrapper.reader.read(&mut buf) {
        Ok(0) => 1,
        Ok(_) => {
            wrapper.reader.seek(SeekFrom::Current(-1)).ok();
            0
        }
        Err(_) => 1,
    }
}
