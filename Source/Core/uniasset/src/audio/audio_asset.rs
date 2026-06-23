use std::{
    cell::UnsafeCell,
    error::Error,
    fmt::Display,
    fs::File,
    io::{self, Cursor, Read, Seek},
    sync::Arc,
};

use parking_lot::RwLock;

use crate::audio::{
    AudioDecoder, AudioFormatProbe, DecoderError, PcmDecoder, SampleFormat, SymphoniaDecoder,
    probe_format_from_stream,
};

/// Concrete decoder variant stored inside [`UnsafeState`].
enum DecoderOption {
    Symphonia(SymphoniaDecoder),
    Pcm(PcmDecoder),
    None,
}

struct SafeState {
    audio_info: Option<AudioInfo>,
}

struct UnsafeState {
    audio_decoder: DecoderOption,
}

pub struct AudioAsset(RwLock<SafeState>, UnsafeCell<UnsafeState>);

impl Default for AudioAsset {
    fn default() -> Self {
        Self(
            RwLock::new(SafeState { audio_info: None }),
            UnsafeCell::new(UnsafeState {
                audio_decoder: DecoderOption::None,
            }),
        )
    }
}

#[derive(Clone)]
struct AudioInfo {
    frame_count: u64,
    sample_count: u64,
    sample_rate: u32,
    channel_count: u16,
    sample_format: SampleFormat,
}

impl AudioAsset {
    #[inline]
    #[allow(clippy::mut_from_ref)]
    fn unsafe_state(&self) -> &mut UnsafeState {
        unsafe { &mut *self.1.get() }
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

        let info = AudioInfo {
            frame_count: decoder.get_frame_count(),
            sample_count: decoder.get_sample_count(),
            sample_rate: decoder.get_sample_rate(),
            channel_count: decoder.get_channel_count() as u16,
            sample_format,
        };

        // Write audio_info to SafeState with lock
        let mut safe_state = self.0.write();
        safe_state.audio_info = Some(info);

        // Write decoder to UnsafeState
        self.unsafe_state().audio_decoder = DecoderOption::Symphonia(decoder);

        Ok(())
    }

    pub fn unload(&self) {
        // Clear SafeState with lock
        let mut safe_state = self.0.write();
        safe_state.audio_info = None;

        // Clear UnsafeState
        self.unsafe_state().audio_decoder = DecoderOption::None;
    }

    pub fn get_channel_count(&self) -> Result<u16, AudioOperationError> {
        let state = self.0.read();
        Ok(state
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .channel_count)
    }

    pub fn get_sample_count(&self) -> Result<u64, AudioOperationError> {
        let state = self.0.read();
        Ok(state
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .sample_count)
    }

    pub fn get_sample_rate(&self) -> Result<u32, AudioOperationError> {
        let state = self.0.read();
        Ok(state
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .sample_rate)
    }

    pub fn get_sample_format(&self) -> Result<SampleFormat, AudioOperationError> {
        let state = self.0.read();
        Ok(state
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .sample_format)
    }

    pub fn get_frame_count(&self) -> Result<u64, AudioOperationError> {
        let state = self.0.read();
        Ok(state
            .audio_info
            .as_ref()
            .ok_or(AudioOperationError::Unloaded)?
            .frame_count)
    }

    pub fn tell(&self) -> Result<i64, AudioOperationError> {
        let _safe_state = self.0.read();
        match &self.unsafe_state().audio_decoder {
            DecoderOption::Symphonia(d) => Ok(d.tell()),
            DecoderOption::Pcm(d) => Ok(d.tell()),
            DecoderOption::None => Err(AudioOperationError::Unloaded),
        }
    }

    pub fn read(&self, buffer: &mut [u8], frame_count: u32) -> Result<u32, AudioOperationError> {
        let _safe_state = self.0.read();
        match &mut self.unsafe_state().audio_decoder {
            DecoderOption::Symphonia(d) => d.read(buffer, frame_count).map_err(Into::into),
            DecoderOption::Pcm(d) => d.read(buffer, frame_count).map_err(Into::into),
            DecoderOption::None => Err(AudioOperationError::Unloaded),
        }
    }

    pub fn seek(&self, position: i64) -> Result<(), AudioOperationError> {
        let _safe_state = self.0.read();
        match &mut self.unsafe_state().audio_decoder {
            DecoderOption::Symphonia(d) => d.seek(position).map_err(Into::into),
            DecoderOption::Pcm(d) => d.seek(position).map_err(Into::into),
            DecoderOption::None => Err(AudioOperationError::Unloaded),
        }
    }

    pub unsafe fn read_unsafe(
        &self,
        buffer: &mut [u8],
        frame_count: u32,
    ) -> Result<u32, AudioOperationError> {
        match &mut self.unsafe_state().audio_decoder {
            DecoderOption::Symphonia(d) => d.read(buffer, frame_count).map_err(Into::into),
            DecoderOption::Pcm(d) => d.read(buffer, frame_count).map_err(Into::into),
            DecoderOption::None => Err(AudioOperationError::Unloaded),
        }
    }

    pub unsafe fn seek_unsafe(&self, position: i64) -> Result<(), AudioOperationError> {
        match &mut self.unsafe_state().audio_decoder {
            DecoderOption::Symphonia(d) => d.seek(position).map_err(Into::into),
            DecoderOption::Pcm(d) => d.seek(position).map_err(Into::into),
            DecoderOption::None => Err(AudioOperationError::Unloaded),
        }
    }

    pub fn prepare(&self) -> Result<(), AudioOperationError> {
        // Take write lock to block concurrent read/seek
        let _safe_state = self.0.write();
        let unsafe_ref = self.unsafe_state();

        let old_decoder = std::mem::replace(&mut unsafe_ref.audio_decoder, DecoderOption::None);

        match old_decoder {
            DecoderOption::Symphonia(mut symph) => {
                // Save current position
                let saved_pos = symph.tell();

                let sample_format = symph.get_sample_format();
                let sample_rate = symph.get_sample_rate();
                let channel_count = symph.get_channel_count();
                let frame_count = symph.get_frame_count();
                let bytes_per_frame = sample_format.byte_size() * channel_count;
                let total_bytes = frame_count as usize * bytes_per_frame;

                symph.seek(0)?;

                let mut pcm_data = vec![0u8; total_bytes];
                let mut offset = 0usize;
                while offset < total_bytes {
                    let remaining_bytes = total_bytes - offset;
                    let remaining_frames = (remaining_bytes / bytes_per_frame) as u32;
                    if remaining_frames == 0 {
                        break;
                    }
                    let to_read = remaining_frames.min(4096);
                    let end = offset + to_read as usize * bytes_per_frame;
                    let buf_slice = &mut pcm_data[offset..end];
                    let frames_read = symph.read(buf_slice, to_read)?;
                    if frames_read == 0 {
                        break;
                    }
                    offset += frames_read as usize * bytes_per_frame;
                }

                let shared_data = Arc::from(pcm_data.into_boxed_slice());
                let mut pcm = PcmDecoder::new(
                    shared_data,
                    sample_format,
                    sample_rate,
                    channel_count,
                    frame_count,
                );

                // Restore position
                pcm.seek(saved_pos)?;

                unsafe_ref.audio_decoder = DecoderOption::Pcm(pcm);
                Ok(())
            }
            DecoderOption::Pcm(pcm) => {
                // Already prepared, put it back
                unsafe_ref.audio_decoder = DecoderOption::Pcm(pcm);
                Ok(())
            }
            DecoderOption::None => {
                unsafe_ref.audio_decoder = DecoderOption::None;
                Err(AudioOperationError::Unloaded)
            }
        }
    }

    pub fn try_clone(&self) -> Result<Self, AudioOperationError> {
        // Take write lock and block the safe version of read and seek.
        let safe_state = self.0.write();
        let safe_clone = safe_state.audio_info.clone();

        // Take ownership of the decoder, process, and write back
        let unsafe_ref = self.unsafe_state();
        let old_decoder = std::mem::replace(&mut unsafe_ref.audio_decoder, DecoderOption::None);

        let clone_decoder = match old_decoder {
            DecoderOption::None => DecoderOption::None,

            DecoderOption::Pcm(pcm) => {
                let cloned = pcm.clone();
                unsafe_ref.audio_decoder = DecoderOption::Pcm(pcm);
                DecoderOption::Pcm(cloned)
            }

            DecoderOption::Symphonia(symph) => {
                // Put the decoder back — must prepare() first
                unsafe_ref.audio_decoder = DecoderOption::Symphonia(symph);
                return Err(AudioOperationError::NotPrepared);
            }
        };

        Ok(Self(
            RwLock::new(SafeState {
                audio_info: safe_clone,
            }),
            UnsafeCell::new(UnsafeState {
                audio_decoder: clone_decoder,
            }),
        ))
    }
}

#[derive(Debug)]
pub enum AudioOperationError {
    IOError(io::Error),
    DecoderError(DecoderError),
    UnsupportedFormat,
    Unloaded,
    NotPrepared,
}

impl Error for AudioOperationError {}

impl Display for AudioOperationError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AudioOperationError::IOError(error) => write!(f, "IO Error: {error}"),
            AudioOperationError::UnsupportedFormat => write!(f, "Unsupported audio format"),
            AudioOperationError::DecoderError(error) => write!(f, "Decoder Error: {error}"),
            AudioOperationError::Unloaded => write!(f, "No audio asset loaded"),
            AudioOperationError::NotPrepared => {
                write!(
                    f,
                    "Audio asset must be prepared before cloning; call prepare() first"
                )
            }
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

#[cfg(test)]
mod tests {
    use super::*;

    fn create_wav_bytes(
        sample_rate: u32,
        channels: u16,
        sample_format: SampleFormat,
        num_frames: u32,
    ) -> Vec<u8> {
        let bits_per_sample = (sample_format.byte_size() * 8) as u16;
        let bytes_per_frame = channels * (bits_per_sample / 8);
        let data_size = num_frames * bytes_per_frame as u32;

        let mut wav = Vec::new();
        wav.extend_from_slice(b"RIFF");
        wav.extend_from_slice(&(36 + data_size).to_le_bytes());
        wav.extend_from_slice(b"WAVE");
        wav.extend_from_slice(b"fmt ");
        wav.extend_from_slice(&16u32.to_le_bytes());

        let format_tag = match sample_format {
            SampleFormat::Float32 => 3u16,
            SampleFormat::Int16 => 1u16,
        };
        wav.extend_from_slice(&format_tag.to_le_bytes());
        wav.extend_from_slice(&channels.to_le_bytes());
        wav.extend_from_slice(&sample_rate.to_le_bytes());

        let byte_rate = sample_rate * bytes_per_frame as u32;
        wav.extend_from_slice(&byte_rate.to_le_bytes());
        wav.extend_from_slice(&bytes_per_frame.to_le_bytes());
        wav.extend_from_slice(&bits_per_sample.to_le_bytes());

        wav.extend_from_slice(b"data");
        wav.extend_from_slice(&data_size.to_le_bytes());

        for i in 0..num_frames {
            for _ch in 0..channels {
                let t = i as f32 / sample_rate as f32;
                let val = (2.0 * std::f32::consts::PI * 440.0 * t).sin() * 0.5;
                match sample_format {
                    SampleFormat::Float32 => wav.extend_from_slice(&val.to_le_bytes()),
                    SampleFormat::Int16 => {
                        let sample = (val * 16000.0) as i16;
                        wav.extend_from_slice(&sample.to_le_bytes());
                    }
                }
            }
        }
        wav
    }

    #[test]
    fn audio_asset_load_wav_float32() {
        let wav_data = create_wav_bytes(44100, 2, SampleFormat::Float32, 100);
        let asset = AudioAsset::default();

        asset.load_memory(wav_data, SampleFormat::Float32).unwrap();

        assert_eq!(asset.get_sample_rate().unwrap(), 44100);
        assert_eq!(asset.get_channel_count().unwrap(), 2);
        assert_eq!(asset.get_frame_count().unwrap(), 100);
        assert_eq!(asset.get_sample_count().unwrap(), 200);
    }

    #[test]
    fn audio_asset_load_wav_int16() {
        let wav_data = create_wav_bytes(16000, 1, SampleFormat::Int16, 80);
        let asset = AudioAsset::default();

        asset.load_memory(wav_data, SampleFormat::Int16).unwrap();

        assert_eq!(asset.get_sample_rate().unwrap(), 16000);
        assert_eq!(asset.get_channel_count().unwrap(), 1);
        assert_eq!(asset.get_frame_count().unwrap(), 80);
    }

    #[test]
    fn audio_asset_unloaded_error() {
        let asset = AudioAsset::default();
        assert!(matches!(
            asset.get_sample_rate(),
            Err(AudioOperationError::Unloaded)
        ));
        assert!(matches!(asset.tell(), Err(AudioOperationError::Unloaded)));
    }

    #[test]
    fn audio_asset_read_and_seek() {
        let wav_data = create_wav_bytes(44100, 2, SampleFormat::Float32, 100);
        let asset = AudioAsset::default();
        asset.load_memory(wav_data, SampleFormat::Float32).unwrap();

        let mut buffer = vec![0u8; 2 * 4 * 10];
        let frames_read = asset.read(&mut buffer, 10).unwrap();
        assert_eq!(frames_read, 10);
        assert_eq!(asset.tell().unwrap(), 10);

        asset.seek(0).unwrap();
        assert_eq!(asset.tell().unwrap(), 0);

        asset.seek(50).unwrap();
        assert_eq!(asset.tell().unwrap(), 50);
    }

    #[test]
    fn audio_asset_prepare() {
        let wav_data = create_wav_bytes(44100, 2, SampleFormat::Float32, 100);
        let asset = AudioAsset::default();
        asset.load_memory(wav_data, SampleFormat::Float32).unwrap();

        asset.prepare().unwrap();

        // After prepare, all properties should still be accessible
        assert_eq!(asset.get_sample_rate().unwrap(), 44100);
        assert_eq!(asset.get_channel_count().unwrap(), 2);
        assert_eq!(asset.get_frame_count().unwrap(), 100);

        // Should be idempotent
        asset.prepare().unwrap();
        assert_eq!(asset.get_sample_rate().unwrap(), 44100);
    }

    #[test]
    fn audio_asset_prepare_unloaded() {
        let asset = AudioAsset::default();
        assert!(matches!(
            asset.prepare(),
            Err(AudioOperationError::Unloaded)
        ));
    }

    #[test]
    fn audio_asset_try_clone() {
        let wav_data = create_wav_bytes(44100, 2, SampleFormat::Float32, 100);
        let asset = AudioAsset::default();
        asset.load_memory(wav_data, SampleFormat::Float32).unwrap();

        asset.seek(25).unwrap();
        asset.prepare().unwrap();

        let cloned = asset.try_clone().unwrap();
        assert_eq!(cloned.get_sample_rate().unwrap(), 44100);
        assert_eq!(cloned.get_channel_count().unwrap(), 2);
        assert_eq!(cloned.tell().unwrap(), 25);

        assert_eq!(asset.tell().unwrap(), 25);
    }

    #[test]
    fn audio_asset_try_clone_without_prepare() {
        let wav_data = create_wav_bytes(44100, 2, SampleFormat::Float32, 100);
        let asset = AudioAsset::default();
        asset.load_memory(wav_data, SampleFormat::Float32).unwrap();

        assert!(matches!(
            asset.try_clone(),
            Err(AudioOperationError::NotPrepared)
        ));
    }

    #[test]
    fn audio_asset_unload() {
        let wav_data = create_wav_bytes(44100, 2, SampleFormat::Float32, 50);
        let asset = AudioAsset::default();
        asset.load_memory(wav_data, SampleFormat::Float32).unwrap();

        assert!(asset.get_sample_rate().is_ok());

        asset.unload();
        assert!(matches!(
            asset.get_sample_rate(),
            Err(AudioOperationError::Unloaded)
        ));
    }

    #[test]
    fn audio_asset_unsupported_format() {
        let asset = AudioAsset::default();
        let invalid_data = vec![0u8; 100];
        let result = asset.load_memory(invalid_data, SampleFormat::Float32);
        assert!(matches!(
            result,
            Err(AudioOperationError::UnsupportedFormat)
        ));
    }

    #[test]
    fn audio_asset_load_from_file() {
        let asset = AudioAsset::default();
        let test_path = concat!(env!("CARGO_MANIFEST_DIR"), "/../../CoreTestAssets/test.wav");
        asset.load_file(test_path, SampleFormat::Float32).unwrap();

        assert_eq!(asset.get_sample_rate().unwrap(), 44100);
        assert_eq!(asset.get_channel_count().unwrap(), 2);
    }
}
