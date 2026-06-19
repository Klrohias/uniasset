use std::{
    cmp::min,
    io,
    sync::{
        atomic::{AtomicU64, Ordering},
        Arc,
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

        buffer[..byte_count].copy_from_slice(&self.data[byte_offset..byte_offset + byte_count]);

        self.position
            .store((current_pos + frames_to_read) as u64, Ordering::Relaxed);

        Ok(frames_to_read as u32)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn pcm_decoder_new() {
        let data: Vec<u8> = (0..400).map(|i| (i % 256) as u8).collect();
        let data_arc = Arc::from(data.into_boxed_slice());

        let decoder = PcmDecoder::new(data_arc, SampleFormat::Float32, 44100, 2, 50);

        assert_eq!(decoder.get_sample_rate(), 44100);
        assert_eq!(decoder.get_channel_count(), 2);
        assert_eq!(decoder.get_frame_count(), 50);
        assert_eq!(decoder.get_sample_count(), 100);
        assert_eq!(decoder.get_sample_format(), SampleFormat::Float32);
        assert_eq!(decoder.tell(), 0);
    }

    #[test]
    fn pcm_decoder_read() {
        let data: Vec<u8> = (0..80).map(|i| (i % 256) as u8).collect();
        let data_arc = Arc::from(data.into_boxed_slice());

        let mut decoder = PcmDecoder::new(data_arc, SampleFormat::Int16, 16000, 1, 40);

        let mut buffer = vec![0u8; 20];
        let frames_read = decoder.read(&mut buffer, 10).unwrap();
        assert_eq!(frames_read, 10);
        assert_eq!(decoder.tell(), 10);

        for i in 0..20 {
            assert_eq!(buffer[i], (i % 256) as u8);
        }
    }

    #[test]
    fn pcm_decoder_seek() {
        let data: Vec<u8> = (0..80).map(|i| (i % 256) as u8).collect();
        let data_arc = Arc::from(data.into_boxed_slice());

        let mut decoder = PcmDecoder::new(data_arc, SampleFormat::Int16, 16000, 1, 40);

        decoder.seek(20).unwrap();
        assert_eq!(decoder.tell(), 20);

        decoder.seek(0).unwrap();
        assert_eq!(decoder.tell(), 0);
    }

    #[test]
    fn pcm_decoder_seek_invalid() {
        let data: Vec<u8> = vec![0u8; 80];
        let data_arc = Arc::from(data.into_boxed_slice());

        let mut decoder = PcmDecoder::new(data_arc, SampleFormat::Int16, 16000, 1, 40);

        assert!(decoder.seek(-1).is_err());
        assert!(decoder.seek(50).is_err());
    }

    #[test]
    fn pcm_decoder_read_beyond_end() {
        let data: Vec<u8> = vec![0u8; 20];
        let data_arc = Arc::from(data.into_boxed_slice());

        let mut decoder = PcmDecoder::new(data_arc, SampleFormat::Int16, 16000, 1, 10);

        decoder.seek(8).unwrap();

        let mut buffer = vec![0u8; 10];
        let frames_read = decoder.read(&mut buffer, 5).unwrap();
        assert_eq!(frames_read, 2);
    }

    #[test]
    fn pcm_decoder_clone() {
        let data: Vec<u8> = (0..80).map(|i| (i % 256) as u8).collect();
        let data_arc = Arc::from(data.into_boxed_slice());

        let mut decoder = PcmDecoder::new(data_arc, SampleFormat::Int16, 16000, 1, 40);

        decoder.seek(15).unwrap();

        let cloned = decoder.clone();
        assert_eq!(cloned.tell(), 15);
        assert_eq!(cloned.get_sample_rate(), 16000);
        assert_eq!(cloned.get_channel_count(), 1);

        let mut buffer1 = vec![0u8; 4];
        let mut buffer2 = vec![0u8; 4];
        decoder.read(&mut buffer1, 2).unwrap();
        cloned.clone().read(&mut buffer2, 2).unwrap();
        assert_eq!(buffer1, buffer2);
    }
}
