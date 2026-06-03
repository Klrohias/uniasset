use std::{
    error::Error,
    fmt::Display,
    fs::File,
    io::{self, Cursor, Read, Seek},
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

pub struct AudioAsset(Box<Arc<SyncUnsafeCell<AudioAssetState>>>);

impl Default for AudioAsset {
    fn default() -> Self {
        Self(Box::new(Arc::new(AudioAssetState::default().into())))
    }
}

impl AudioAsset {
    fn unsafe_mut_state<'a>(&'a self) -> &'a mut AudioAssetState {
        unsafe { &mut *self.0.get() }
    }

    pub fn load_file(&self, path: impl AsRef<str>) -> Result<(), AudioOperationError> {
        self.load_io(File::open(path.as_ref())?)
    }

    pub fn load_memory(
        &self,
        data: impl AsRef<[u8]> + Send + Sync + 'static,
    ) -> Result<(), AudioOperationError> {
        self.load_io(Cursor::new(data))
    }

    pub fn load_io(
        &self,
        mut stream: impl Read + Seek + Send + Sync + 'static,
    ) -> Result<(), AudioOperationError> {
        if probe_format_from_stream(&mut stream)? == AudioFormatProbe::Unsupported {
            return Err(AudioOperationError::UnsupportedFormat);
        }

        let decoder = SymphoniaDecoder::from_io(stream, SampleFormat::Int16)?;
        let state = self.unsafe_mut_state();
        let _guard = state.lock.write();

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
        state
            .audio_decoder
            .store(Box::into_raw(boxed_decoder_wrapper), Ordering::Relaxed);

        Ok(())
    }

    pub fn get_channel_count(&self) -> Result<u16, AudioOperationError> {
        let state = self.unsafe_mut_state();
        let _guard = state.lock.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .channel_count)
    }

    pub fn get_sample_count(&self) -> Result<u64, AudioOperationError> {
        let state = self.unsafe_mut_state();
        let _guard = state.lock.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .sample_count)
    }

    pub fn get_sample_rate(&self) -> Result<u32, AudioOperationError> {
        let state = self.unsafe_mut_state();
        let _guard = state.lock.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .sample_rate)
    }

    pub fn get_frame_count(&self) -> Result<u64, AudioOperationError> {
        let state = self.unsafe_mut_state();
        let _guard = state.lock.read();

        Ok(state
            .audio_info
            .as_ref()
            .ok_or_else(|| AudioOperationError::Unloaded)?
            .frame_count)
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
struct AudioAssetState {
    lock: RwLock<()>,
    audio_info: Option<AudioInfo>,
    audio_decoder: AtomicPtr<AudioDecoderWrapper>,
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
