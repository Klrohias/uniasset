use std::{
    cmp::min,
    io,
    sync::{
        Arc,
        atomic::{AtomicU64, Ordering},
    },
};

use crate::audio::{AudioDecoder, DecoderError, SampleFormat};

/// A lock-free, cloneable PCM decoder backed by shared memory.
///
/// Stores interleaved PCM sample data in an `Arc<[u8]>`. Multiple clones share
/// the same underlying data; each maintains its own atomic read position.
pub struct PcmDecoder {
    data: Arc<[u8]>,
    sample_format: SampleFormat,
    sample_rate: u32,
    channel_count: usize,
    frame_count: u64,
    bytes_per_frame: usize,
    position: AtomicU64,
}

impl PcmDecoder {
    /// Create a new PCM decoder from shared interleaved PCM data.
    ///
    /// # Panics
    /// Panics if `data.len()` is less than `frame_count * bytes_per_frame`.
    pub fn new(
        data: Arc<[u8]>,
        sample_format: SampleFormat,
        sample_rate: u32,
        channel_count: usize,
        frame_count: u64,
    ) -> Self {
        let bytes_per_frame = sample_format.byte_size() * channel_count;
        assert!(
            data.len() >= frame_count as usize * bytes_per_frame,
            "PcmDecoder data too small: {} bytes for {} frames * {} bytes/frame",
            data.len(),
            frame_count,
            bytes_per_frame,
        );
        Self {
            data,
            sample_format,
            sample_rate,
            channel_count,
            frame_count,
            bytes_per_frame,
            position: AtomicU64::new(0),
        }
    }
}

impl Clone for PcmDecoder {
    fn clone(&self) -> Self {
        Self {
            data: Arc::clone(&self.data),
            sample_format: self.sample_format,
            sample_rate: self.sample_rate,
            channel_count: self.channel_count,
            frame_count: self.frame_count,
            bytes_per_frame: self.bytes_per_frame,
            position: AtomicU64::new(self.position.load(Ordering::Relaxed)),
        }
    }
}

impl AudioDecoder for PcmDecoder {
    fn get_sample_format(&self) -> SampleFormat {
        self.sample_format
    }

    fn get_frame_count(&self) -> u64 {
        self.frame_count
    }

    fn get_sample_count(&self) -> u64 {
        self.frame_count * self.channel_count as u64
    }

    fn get_sample_rate(&self) -> u32 {
        self.sample_rate
    }

    fn get_channel_count(&self) -> usize {
        self.channel_count
    }

    fn tell(&self) -> i64 {
        self.position.load(Ordering::Relaxed) as i64
    }

    fn seek(&mut self, position: i64) -> Result<(), DecoderError> {
        if position < 0 {
            return Err(DecoderError::IOError(io::Error::new(
                io::ErrorKind::InvalidInput,
                "negative seek position is invalid",
            )));
        }
        let position = position as u64;
        if position >= self.frame_count {
            return Err(DecoderError::IOError(io::Error::new(
                io::ErrorKind::InvalidInput,
                "seek position exceeds total frame count",
            )));
        }
        self.position.store(position, Ordering::Relaxed);
        Ok(())
    }

    fn read(&mut self, buffer: &mut [u8], frame_count: u32) -> Result<u32, DecoderError> {
        let current_pos = self.position.load(Ordering::Relaxed) as usize;
        let available_frames = self.frame_count.saturating_sub(current_pos as u64) as usize;
        let frames_to_read = min(frame_count as usize, available_frames);

        if frames_to_read == 0 {
            return Ok(0);
        }

        let byte_offset = current_pos * self.bytes_per_frame;
        let byte_count = frames_to_read * self.bytes_per_frame;

        buffer[..byte_count]
            .copy_from_slice(&self.data[byte_offset..byte_offset + byte_count]);

        self.position
            .store((current_pos + frames_to_read) as u64, Ordering::Relaxed);

        Ok(frames_to_read as u32)
    }
}
