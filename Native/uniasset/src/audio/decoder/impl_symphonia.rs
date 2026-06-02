use std::{cmp::min, io, sync::Arc};

use symphonia::core::{
    codecs::audio::{AudioDecoderOptions, CODEC_ID_NULL_AUDIO},
    errors::Error as SymphoniaError,
    formats::{FormatOptions, FormatReader, SeekMode, SeekTo, probe::Hint},
    io::{MediaSource, MediaSourceStream, MediaSourceStreamOptions},
    meta::MetadataOptions,
    units::{Time, TimeBase, Timestamp},
};
use symphonia::default::{get_codecs, get_probe};

use crate::audio::{AudioDecoder, DecoderError, SampleFormat};
struct DecoderInner {
    reader: Box<dyn FormatReader>,
    decoder: Box<dyn symphonia::core::codecs::audio::AudioDecoder>,
    track_id: u32,
    time_base: Option<TimeBase>,
    reached_eof: bool,
    remain_buffer: Option<PcmFrameBuffer>,
}

pub struct SymphoniaDecoder {
    sample_format: SampleFormat,
    sample_count: u64,
    sample_rate: u32,
    channel_count: usize,
    bytes_per_frame: usize,
    position: u64,

    inner: DecoderInner,
}

impl SymphoniaDecoder {
    pub fn from_io<T>(stream: T, sample_format: SampleFormat) -> Result<Self, DecoderError>
    where
        T: MediaSource + 'static,
    {
        let mss = MediaSourceStream::new(Box::new(stream), MediaSourceStreamOptions::default());
        Self::from_media_source_stream(mss, sample_format)
    }

    pub fn from_memory<T>(data: T, sample_format: SampleFormat) -> Result<Self, DecoderError>
    where
        T: AsRef<[u8]> + Send + Sync + 'static,
    {
        Self::from_io(io::Cursor::new(data), sample_format)
    }

    fn from_media_source_stream(
        mss: MediaSourceStream<'static>,
        sample_format: SampleFormat,
    ) -> Result<Self, DecoderError> {
        let hint = Hint::new();
        let format = get_probe().probe(
            &hint,
            mss,
            FormatOptions::default(),
            MetadataOptions::default(),
        )?;

        let track = format
            .default_track(symphonia::core::formats::TrackType::Audio)
            .ok_or(DecoderError::NoTrack)?;

        let codec_params = track
            .codec_params
            .as_ref()
            .and_then(|x| x.audio())
            .ok_or(DecoderError::NoTrack)?;

        if codec_params.codec == CODEC_ID_NULL_AUDIO {
            return Err(DecoderError::UnsupportedFormat);
        }

        let track_id = track.id;
        let sample_rate = codec_params
            .sample_rate
            .ok_or(DecoderError::UnsupportedFormat)?;
        let channel_count = codec_params
            .channels
            .as_ref()
            .map(|x| x.count())
            .ok_or(DecoderError::UnsupportedFormat)?;
        let time_base = track.time_base;
        let sample_count = track.num_frames.unwrap_or(0);

        let decoder =
            get_codecs().make_audio_decoder(codec_params, &AudioDecoderOptions::default())?;

        let bytes_per_frame = sample_format.byte_size() * channel_count;

        Ok(Self {
            sample_format,
            sample_count,
            sample_rate,
            channel_count,
            bytes_per_frame,
            position: 0,
            inner: DecoderInner {
                reader: format,
                decoder,
                track_id,
                time_base,
                reached_eof: false,
                remain_buffer: None,
            },
        })
    }

    /// Decode the next packet from the format reader.
    /// Returns Ok(None) at EOF. Errors from unrecognized/corrupt packets are silently skipped.
    fn decode_next_packet(&mut self) -> Result<Option<PcmFrameBuffer>, DecoderError> {
        if self.inner.reached_eof {
            return Ok(None);
        }

        // Loop until we get a valid decoded frame or hit EOF/error
        loop {
            let packet = match self.inner.reader.next_packet() {
                Ok(Some(packet)) => packet,
                Ok(None) => {
                    self.inner.reached_eof = true;
                    return Ok(None);
                }
                Err(SymphoniaError::IoError(e)) => return Err(DecoderError::IOError(e)),
                Err(_) => continue,
            };

            if packet.track_id != self.inner.track_id {
                continue;
            }

            let decoded = match self.inner.decoder.decode(&packet) {
                Ok(decoded) => decoded,
                Err(SymphoniaError::DecodeError(_)) => continue,
                Err(SymphoniaError::ResetRequired) => {
                    self.inner.decoder.reset();
                    continue;
                }
                Err(SymphoniaError::IoError(e)) => return Err(DecoderError::IOError(e)),
                Err(_) => continue,
            };

            let pcm_data: Box<[u8]> = match self.sample_format {
                SampleFormat::Float32 => {
                    let mut interleaved = vec![0f32; decoded.samples_interleaved()];
                    decoded.copy_to_slice_interleaved::<f32, _>(interleaved.as_mut_slice());
                    bytemuck::cast_vec(interleaved).into_boxed_slice()
                }
                SampleFormat::Int16 => {
                    let mut interleaved = vec![0i16; decoded.samples_interleaved()];
                    decoded.copy_to_slice_interleaved::<i16, _>(interleaved.as_mut_slice());
                    bytemuck::cast_vec(interleaved).into_boxed_slice()
                }
            };

            return Ok(Some(PcmFrameBuffer {
                data: Arc::from(pcm_data),
                offset: 0,
            }));
        }
    }

    /// Convert a sample position to a `Timestamp` for precise seeking.
    ///
    /// Uses direct rational conversion: `ts = sample * denom / (numer * sample_rate)`.
    /// This avoids the Time intermediate and its associated precision loss
    /// from integer division in the nanoseconds conversion.
    ///
    /// Returns `None` if no time base is available or the value overflows `i64`.
    fn sample_to_timestamp(&self, sample: u64) -> Option<Timestamp> {
        let tb = self.inner.time_base?;
        // ts = sample * denom / (numer * sample_rate)
        let ts = sample as u128 * tb.denom.get() as u128
            / (tb.numer.get() as u128 * self.sample_rate as u128);
        i64::try_from(ts).ok().map(Timestamp::new)
    }

    /// Convert a `Timestamp` to sample position using direct rational conversion.
    ///
    /// Formula: `sample = ts * numer * sample_rate / denom`.
    /// No Time or nanosecond intermediate — single mul+div path.
    fn timestamp_to_sample(&self, ts: Timestamp) -> u64 {
        match self.inner.time_base {
            Some(tb) => {
                let ts = ts.get();
                if ts <= 0 {
                    return 0;
                }
                // sample = ts * numer * sample_rate / denom
                (ts as u128 * tb.numer.get() as u128 * self.sample_rate as u128
                    / tb.denom.get() as u128) as u64
            }
            None => {
                // Without a time base, assume timestamp is already in sample units
                ts.get().max(0) as u64
            }
        }
    }

    /// Fallback: sample → Time (seconds + nanos). Only used when time base is unavailable.
    fn sample_to_time(&self, sample: u64) -> Option<Time> {
        let seconds = (sample / self.sample_rate as u64) as i64;
        let remainder = sample % self.sample_rate as u64;
        let nanos = (remainder * 1_000_000_000 / self.sample_rate as u64) as u32;
        Time::try_new(seconds, nanos)
    }
}

impl AudioDecoder for SymphoniaDecoder {
    fn get_sample_format(&self) -> SampleFormat {
        self.sample_format
    }

    fn get_sample_count(&self) -> u64 {
        self.sample_count
    }

    fn get_sample_rate(&self) -> u32 {
        self.sample_rate
    }

    fn get_channel_count(&self) -> usize {
        self.channel_count
    }

    fn tell(&self) -> i64 {
        if self.bytes_per_frame == 0 {
            return 0;
        }
        self.position as i64
    }

    fn seek(&mut self, position: i64) -> Result<(), DecoderError> {
        if position < 0 {
            return Err(DecoderError::IOError(io::Error::new(
                io::ErrorKind::InvalidInput,
                "negative seek position is invalid",
            )));
        }

        let target_sample = position as u64;

        // If seeking to current position, nothing to do
        if target_sample == self.position && self.inner.remain_buffer.is_none() {
            return Ok(());
        }

        // Seek to position 0: fast path
        if target_sample == 0 {
            self.inner.reader.seek(
                SeekMode::Accurate,
                SeekTo::Timestamp {
                    ts: Timestamp::ZERO,
                    track_id: self.inner.track_id,
                },
            )?;
            self.inner.decoder.reset();
            self.inner.remain_buffer = None;
            self.inner.reached_eof = false;
            self.position = 0;
            return Ok(());
        }

        // Prefer Timestamp-based seek for maximal precision.
        // This avoids the Time roundtrip: Sample → Time(ns loss) → [symphonia: Time→Timestamp].
        // Instead: Sample → Timestamp (single rational conversion, no intermediate).
        let seeked = if let Some(ts) = self.sample_to_timestamp(target_sample) {
            self.inner.reader.seek(
                SeekMode::Accurate,
                SeekTo::Timestamp {
                    ts,
                    track_id: self.inner.track_id,
                },
            )?
        } else {
            // Fallback: no time base or timestamp overflowed i64.
            // Use Time-based seek with nanosecond precision.
            let target_time = self.sample_to_time(target_sample).ok_or_else(|| {
                DecoderError::IOError(io::Error::new(
                    io::ErrorKind::InvalidInput,
                    "seek position out of time range",
                ))
            })?;

            self.inner.reader.seek(
                SeekMode::Accurate,
                SeekTo::Time {
                    time: target_time,
                    track_id: Some(self.inner.track_id),
                },
            )?
        };

        self.inner.decoder.reset();
        self.inner.remain_buffer = None;
        self.inner.reached_eof = false;

        // Use the actual timestamp reached to set the exact sample position
        self.position = self.timestamp_to_sample(seeked.actual_ts);

        Ok(())
    }

    fn read(&mut self, buffer: &mut [u8], count: u32) -> Result<u32, DecoderError> {
        if self.bytes_per_frame == 0 || buffer.is_empty() || count == 0 {
            return Ok(0);
        }

        let max_bytes = min(count as usize, buffer.len());
        let mut written = 0usize;

        // 1. Drain any remaining data from the previous decoded frame.
        //    Reads are rounded down to complete frames to keep position tracking accurate.
        if let Some(ref mut remain) = self.inner.remain_buffer {
            let remain_bytes = remain.remaining();
            // Only copy complete frames
            let to_copy = min(max_bytes, remain_bytes);
            let to_copy = (to_copy / self.bytes_per_frame) * self.bytes_per_frame;

            if to_copy > 0 {
                let start = remain.offset;
                buffer[..to_copy].copy_from_slice(&remain.data[start..start + to_copy]);
                remain.offset += to_copy;
                written += to_copy;
                self.position += (to_copy / self.bytes_per_frame) as u64;
            }

            if remain.remaining() == 0 {
                self.inner.remain_buffer = None;
            }

            if written >= max_bytes {
                return Ok(written as u32);
            }
        }

        // 2. Decode packets on demand until we've filled the buffer or hit EOF
        while written < max_bytes {
            let decoded = self.decode_next_packet()?;
            let decoded = match decoded {
                Some(d) => d,
                None => break, // EOF
            };

            let decoded_bytes = decoded.data.len();
            let needed = max_bytes - written;
            // Only copy complete frames
            let to_copy = min(needed, decoded_bytes);
            let to_copy = (to_copy / self.bytes_per_frame) * self.bytes_per_frame;

            if to_copy > 0 {
                buffer[written..written + to_copy].copy_from_slice(&decoded.data[..to_copy]);

                let frames_copied = to_copy / self.bytes_per_frame;
                written += to_copy;
                self.position += frames_copied as u64;

                // Store leftover as remain buffer for next read
                if to_copy < decoded_bytes {
                    self.inner.remain_buffer = Some(PcmFrameBuffer {
                        data: decoded.data,
                        offset: to_copy,
                    });
                }
            } else {
                // Buffer is too small for even one frame — store the decoded frame
                // and return what we have so far
                self.inner.remain_buffer = Some(decoded);
                return Ok(written as u32);
            }
        }

        Ok(written as u32)
    }
}

struct PcmFrameBuffer {
    data: Arc<[u8]>,
    offset: usize,
}

impl PcmFrameBuffer {
    fn remaining(&self) -> usize {
        self.data.len().saturating_sub(self.offset)
    }
}

impl From<SymphoniaError> for DecoderError {
    fn from(value: SymphoniaError) -> Self {
        match value {
            SymphoniaError::IoError(error) => Self::IOError(error),
            SymphoniaError::Unsupported(_) => Self::UnsupportedFormat,
            other => DecoderError::IOError(io::Error::new(io::ErrorKind::InvalidData, other)),
        }
    }
}
