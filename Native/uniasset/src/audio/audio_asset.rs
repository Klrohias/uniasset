use std::{
    error::Error,
    fmt::Display,
    fs::File,
    io::{self, Read, Seek},
    sync::{Arc, RwLock},
};

use crate::{
    audio::{AudioFormatProbe, probe_format_from_stream},
    native::{NativeHandle, NativeHandleExts},
};

#[derive(Default)]
pub struct AudioAsset(Box<Arc<RwLock<AudioAssetState>>>);

impl AudioAsset {
    pub fn load_file(&self, path: impl AsRef<str>) -> Result<(), AudioOperationError> {
        self.load_io(File::open(path.as_ref())?)
    }

    pub fn load_memory(&self, data: impl AsRef<u8>) -> Result<(), AudioOperationError> {
        todo!()
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
pub struct AudioAssetState {}

#[derive(Debug)]
pub enum AudioOperationError {
    IOError(io::Error),
    UnsupportedFormat,
}

impl Error for AudioOperationError {}

impl Display for AudioOperationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AudioOperationError::IOError(error) => write!(f, "IO Error: {error}"),
            AudioOperationError::UnsupportedFormat => write!(f, "Unsupported audio format"),
        }
    }
}

impl From<io::Error> for AudioOperationError {
    fn from(value: io::Error) -> Self {
        Self::IOError(value)
    }
}
