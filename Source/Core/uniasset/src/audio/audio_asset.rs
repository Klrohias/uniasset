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
}

impl AudioAsset {
    #[inline]
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

            DecoderOption::Symphonia(mut symph) => {
                // Record current position
                let saved_pos = symph.tell();

                // Extract metadata before consuming the decoder
                let sample_format = symph.get_sample_format();
                let sample_rate = symph.get_sample_rate();
                let channel_count = symph.get_channel_count();
                let frame_count = symph.get_frame_count();
                let bytes_per_frame = sample_format.byte_size() * channel_count;
                let total_bytes = frame_count as usize * bytes_per_frame;

                // Seek to start and read all frames
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

                // Wrap in Arc<[u8]>
                let shared_data = Arc::from(pcm_data.into_boxed_slice());

                // Create two PcmDecoders sharing the same backing data
                let mut pcm_self = PcmDecoder::new(
                    Arc::clone(&shared_data),
                    sample_format,
                    sample_rate,
                    channel_count,
                    frame_count,
                );

                let mut pcm_clone = PcmDecoder::new(
                    shared_data,
                    sample_format,
                    sample_rate,
                    channel_count,
                    frame_count,
                );

                // Seek both to the saved position
                pcm_self.seek(saved_pos)?;
                pcm_clone.seek(saved_pos)?;

                // Replace self's decoder with Pcm variant
                unsafe_ref.audio_decoder = DecoderOption::Pcm(pcm_self);

                DecoderOption::Pcm(pcm_clone)
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
