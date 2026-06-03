use std::{error::Error, fmt::Display, io, sync::Arc};

use crate::{
    audio::SampleFormat,
    ffi::{NativeHandle, NativeHandleExts},
    thread::SyncUnsafeCell,
};

pub trait AudioDecoder {
    fn get_sample_format(&self) -> SampleFormat;

    /// Pcm frame count
    fn get_frame_count(&self) -> u64;

    /// SampleCount = FrameCount * ChannelCount
    fn get_sample_count(&self) -> u64;

    fn get_sample_rate(&self) -> u32;
    fn get_channel_count(&self) -> usize;
    fn tell(&self) -> i64;
    fn seek(&mut self, position: i64) -> Result<(), DecoderError>;
    fn read(&mut self, buffer: &mut [u8], frame_count: u32) -> Result<u32, DecoderError>;
}

#[derive(Clone)]
pub struct AudioDecoderWrapper(Box<Arc<SyncUnsafeCell<dyn AudioDecoder + 'static>>>);

impl<T: AudioDecoder + 'static> From<T> for AudioDecoderWrapper {
    fn from(value: T) -> Self {
        Self(Box::new(Arc::new(SyncUnsafeCell::new(value))))
    }
}

impl AudioDecoderWrapper {
    pub fn get_sample_format(&self) -> SampleFormat {
        unsafe { &*self.0.get() }.get_sample_format()
    }

    pub fn get_channel_count(&self) -> usize {
        unsafe { &*self.0.get() }.get_channel_count()
    }

    pub fn get_sample_count(&self) -> u64 {
        unsafe { &*self.0.get() }.get_sample_count()
    }

    pub fn get_sample_rate(&self) -> u32 {
        unsafe { &*self.0.get() }.get_sample_rate()
    }
}

impl NativeHandleExts for AudioDecoderWrapper {
    fn into_handle(self) -> NativeHandle {
        self.0.into_handle()
    }

    fn from_handle(handle: NativeHandle) -> Self {
        Self(NativeHandleExts::from_handle(handle))
    }
}

#[derive(Debug)]
pub enum DecoderError {
    IOError(io::Error),
    UnsupportedFormat,
    NoTrack,
}

impl Error for DecoderError {}

impl Display for DecoderError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            DecoderError::IOError(error) => write!(f, "IO Error: {error}"),
            DecoderError::UnsupportedFormat => write!(f, "Unsupported format"),
            DecoderError::NoTrack => write!(f, "No audio track found"),
        }
    }
}

impl From<io::Error> for DecoderError {
    fn from(value: io::Error) -> Self {
        Self::IOError(value)
    }
}
