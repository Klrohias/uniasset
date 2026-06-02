use std::{
    error::Error,
    fmt::Display,
    fs::File,
    io::{self, Cursor, Read, Seek},
    sync::Arc,
};

use parking_lot::RwLock;

use crate::{
    audio::{
        AudioDecoder, AudioDecoderWrapper, AudioFormatProbe, DecoderError, SampleFormat,
        SymphoniaDecoder, probe_format_from_stream,
    },
    ffi::{NativeHandle, NativeHandleExts},
};

#[derive(Default)]
pub struct AudioAsset(Box<Arc<RwLock<AudioAssetState>>>);

impl AudioAsset {
    pub fn load_file(&self, path: impl AsRef<str>) -> Result<(), AudioOperationError> {
        self.load_io(File::open(path.as_ref())?)
    }

    pub fn load_memory(&self, data: Arc<[u8]>) -> Result<(), AudioOperationError> {
        let decoder = SymphoniaDecoder::from_io(Cursor::new(data.clone()), SampleFormat::Int16)?;

        let info = AudioInfo {
            frame_count: decoder.get_sample_count() * decoder.get_channel_count() as u64,
            sample_count: decoder.get_sample_count(),
            sample_rate: decoder.get_sample_rate(),
            channel_count: decoder.get_channel_count() as u16,
        };

        let mut state = self.0.write();
        state.audio_info = Some(info);

        Ok(())
    }

    pub fn load_io(&self, mut stream: impl Read + Seek) -> Result<(), AudioOperationError> {
        match probe_format_from_stream(&mut stream)? {
            AudioFormatProbe::Mp3 => todo!(),
            AudioFormatProbe::Flac => todo!(),
            AudioFormatProbe::Wav => todo!(),
            AudioFormatProbe::OggVorbis => todo!(),
            AudioFormatProbe::Unsupported => {
                return Err(AudioOperationError::UnsupportedFormat);
            }
        }
    }

    pub fn get_decoder(
        &self,
        sample_format: SampleFormat,
    ) -> Result<AudioDecoderWrapper, DecoderError> {
        let decoder = SymphoniaDecoder::from_memory([], sample_format)?;

        Ok(AudioDecoderWrapper::from(decoder))
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
pub struct AudioAssetState {
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
}

impl Error for AudioOperationError {}

impl Display for AudioOperationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AudioOperationError::IOError(error) => write!(f, "IO Error: {error}"),
            AudioOperationError::UnsupportedFormat => write!(f, "Unsupported audio format"),
            AudioOperationError::DecoderError(error) => write!(f, "Decoder Error: {error}"),
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
