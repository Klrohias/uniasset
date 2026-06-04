use std::{
    error::Error,
    fmt::Display,
    fs::File,
    io::{self, Cursor, Read, Seek},
    ptr::null_mut,
    sync::{
        Arc,
        atomic::{AtomicPtr, Ordering},
    },
};

use parking_lot::RwLock;

use crate::{
    audio::{
        AudioDecoder, AudioDecoderWrapper, AudioFormatProbe, DecoderError, SampleFormat,
        SymphoniaDecoder, probe_format_from_stream,
    },
    ffi::{NativeHandle, NativeHandleExts},
    thread::SyncUnsafeCell,
};

pub struct AudioAsset(Box<Arc<(RwLock<SafeState>, SyncUnsafeCell<UnsafeState>)>>);

impl Default for AudioAsset {
    fn default() -> Self {
        Self(Box::new(Arc::new((
            SafeState::default().into(),
            UnsafeState::default().into(),
        ))))
    }
}

impl AudioAsset {
    fn unsafe_state<'a>(&'a self) -> &'a mut UnsafeState {
        unsafe { &mut *self.0.1.get() }
    }

    fn replace_decoder(&self, new_decoder: *mut AudioDecoderWrapper) {
        let old_ptr = self
            .unsafe_state()
            .audio_decoder
            .swap(new_decoder, Ordering::Release);

        if !old_ptr.is_null() {
            _ = unsafe { Box::from_raw(old_ptr) };
        }
    }

    fn load_decoder(&self) -> Result<*mut AudioDecoderWrapper, AudioOperationError> {
        let decoder_ptr = self.unsafe_state().audio_decoder.load(Ordering::Acquire);

        if decoder_ptr.is_null() {
            return Err(AudioOperationError::Unloaded);
        }

        Ok(decoder_ptr)
    }

    pub fn load_file(
        &self,
        path: impl AsRef<str>,
        sample_format: SampleFormat,
    ) -> Result<(), AudioOperationError> {
        self.load_io(File::open(path.as_ref())?, sample_format)
    }

    pub fn load_memory(
        &self,
        data: impl AsRef<[u8]> + Send + Sync + 'static,
        sample_format: SampleFormat,
    ) -> Result<(), AudioOperationError> {
        self.load_io(Cursor::new(data), sample_format)
    }

    pub fn load_io(
        &self,
        mut stream: impl Read + Seek + Send + Sync + 'static,
        sample_format: SampleFormat,
    ) -> Result<(), AudioOperationError> {
        if probe_format_from_stream(&mut stream)? == AudioFormatProbe::Unsupported {
            return Err(AudioOperationError::UnsupportedFormat);
        }

        let decoder = SymphoniaDecoder::from_io(stream, sample_format)?;
        let mut state = self.0.0.write();

        // Read audio info
        let info = AudioInfo {
            frame_count: decoder.get_frame_count(),
            sample_count: decoder.get_sample_count(),
            sample_rate: decoder.get_sample_rate(),
            channel_count: decoder.get_channel_count() as u16,
        };
        state.audio_info = Some(info);

        // Store audio decoder
        let boxed_decoder_wrapper = Box::new(decoder.into());
        self.replace_decoder(Box::into_raw(boxed_decoder_wrapper));

        Ok(())
    }

    pub fn unload(&self) {
        let mut state = self.0.0.write();
        state.audio_info = None;
        self.replace_decoder(null_mut());
    }

    pub fn get_channel_count(&self) -> Result<u16, AudioOperationError> {
        let state = self.0.0.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .channel_count)
    }

    pub fn get_sample_count(&self) -> Result<u64, AudioOperationError> {
        let state = self.0.0.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .sample_count)
    }

    pub fn get_sample_rate(&self) -> Result<u32, AudioOperationError> {
        let state = self.0.0.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .sample_rate)
    }

    pub fn get_frame_count(&self) -> Result<u64, AudioOperationError> {
        let state = self.0.0.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .frame_count)
    }

    pub fn tell(&self) -> Result<i64, AudioOperationError> {
        Ok(unsafe { &*self.load_decoder()? }.tell())
    }

    pub fn read(&self, buffer: &mut [u8], frame_count: u32) -> Result<u32, AudioOperationError> {
        unsafe { &*self.load_decoder()? }
            .read(buffer, frame_count)
            .map_err(Into::into)
    }

    pub fn seek(&self, position: i64) -> Result<(), AudioOperationError> {
        unsafe { &*self.load_decoder()? }
            .seek(position)
            .map_err(Into::into)
    }
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
struct UnsafeState {
    audio_decoder: AtomicPtr<AudioDecoderWrapper>,
}

#[derive(Default)]
struct SafeState {
    audio_info: Option<AudioInfo>,
}

struct AudioInfo {
    frame_count: u64,
    sample_count: u64,
    sample_rate: u32,
    channel_count: u16,
}

#[derive(Debug)]
pub enum AudioOperationError {
    IOError(io::Error),
    DecoderError(DecoderError),
    UnsupportedFormat,
    Unloaded,
}

impl Error for AudioOperationError {}

impl Display for AudioOperationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AudioOperationError::IOError(error) => write!(f, "IO Error: {error}"),
            AudioOperationError::UnsupportedFormat => write!(f, "Unsupported audio format"),
            AudioOperationError::DecoderError(error) => write!(f, "Decoder Error: {error}"),
            AudioOperationError::Unloaded => write!(f, "No audio asset loaded"),
        }
    }
}

impl From<io::Error> for AudioOperationError {
    fn from(value: io::Error) -> Self {
        Self::IOError(value)
    }
}

impl From<DecoderError> for AudioOperationError {
    fn from(value: DecoderError) -> Self {
        match value {
            DecoderError::UnsupportedFormat => AudioOperationError::UnsupportedFormat,
            other => AudioOperationError::DecoderError(other),
        }
    }
}
