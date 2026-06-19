use std::{
    error::Error,
    fmt::Display,
    fs::File,
    io::{self, Cursor, Read, Seek},
};

use crate::audio::{
    AudioDecoder, AudioDecoderWrapper, AudioFormatProbe, DecoderError, SampleFormat,
    SymphoniaDecoder, probe_format_from_stream,
};

pub struct AudioAsset {
    audio_info: Option<AudioInfo>,
    audio_decoder: Option<AudioDecoderWrapper>,
}

impl Default for AudioAsset {
    fn default() -> Self {
        Self {
            audio_info: None,
            audio_decoder: None,
        }
    }
}

struct AudioInfo {
    frame_count: u64,
    sample_count: u64,
    sample_rate: u32,
    channel_count: u16,
}

impl AudioAsset {
    fn get_decoder(&self) -> Result<&AudioDecoderWrapper, AudioOperationError> {
        self.audio_decoder.as_ref().ok_or(AudioOperationError::Unloaded)
    }

    pub fn load_file(
        &mut self,
        path: impl AsRef<str>,
        sample_format: SampleFormat,
    ) -> Result<(), AudioOperationError> {
        self.load_io(File::open(path.as_ref())?, sample_format)
    }

    pub fn load_memory(
        &mut self,
        data: impl AsRef<[u8]> + Send + Sync + 'static,
        sample_format: SampleFormat,
    ) -> Result<(), AudioOperationError> {
        self.load_io(Cursor::new(data), sample_format)
    }

    pub fn load_io(
        &mut self,
        mut stream: impl Read + Seek + Send + Sync + 'static,
        sample_format: SampleFormat,
    ) -> Result<(), AudioOperationError> {
        if probe_format_from_stream(&mut stream)? == AudioFormatProbe::Unsupported {
            return Err(AudioOperationError::UnsupportedFormat);
        }

        let decoder = SymphoniaDecoder::from_io(stream, sample_format)?;

        // Read audio info
        let info = AudioInfo {
            frame_count: decoder.get_frame_count(),
            sample_count: decoder.get_sample_count(),
            sample_rate: decoder.get_sample_rate(),
            channel_count: decoder.get_channel_count() as u16,
        };
        self.audio_info = Some(info);

        // Store audio decoder
        self.audio_decoder = Some(decoder.into());

        Ok(())
    }

    pub fn unload(&mut self) {
        self.audio_info = None;
        self.audio_decoder = None;
    }

    pub fn get_channel_count(&self) -> Result<u16, AudioOperationError> {
        Ok(self
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .channel_count)
    }

    pub fn get_sample_count(&self) -> Result<u64, AudioOperationError> {
        Ok(self
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .sample_count)
    }

    pub fn get_sample_rate(&self) -> Result<u32, AudioOperationError> {
        Ok(self
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .sample_rate)
    }

    pub fn get_frame_count(&self) -> Result<u64, AudioOperationError> {
        Ok(self
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .frame_count)
    }

    pub fn tell(&self) -> Result<i64, AudioOperationError> {
        Ok(self.get_decoder()?.tell())
    }

    pub fn read(&self, buffer: &mut [u8], frame_count: u32) -> Result<u32, AudioOperationError> {
        self.get_decoder()?
            .read(buffer, frame_count)
            .map_err(Into::into)
    }

    pub fn seek(&self, position: i64) -> Result<(), AudioOperationError> {
        self.get_decoder()?
            .seek(position)
            .map_err(Into::into)
    }
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
