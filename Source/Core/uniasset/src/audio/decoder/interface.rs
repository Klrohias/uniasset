use std::{error::Error, fmt::Display, io};

use crate::audio::SampleFormat;

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
