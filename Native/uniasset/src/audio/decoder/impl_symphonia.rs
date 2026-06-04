use std::{
    cmp::min,
    io::{self, Cursor, Read, Seek},
    sync::atomic::{AtomicI64, Ordering},
};

use parking_lot::Mutex;
use symphonia::core::{
    audio::GenericAudioBufferRef,
    codecs::audio::{AudioDecoderOptions, CODEC_ID_NULL_AUDIO},
    errors::Error as SymphoniaError,
    formats::{FormatOptions, FormatReader, SeekMode, SeekTo, probe::Hint},
    io::{MediaSource, MediaSourceStream, MediaSourceStreamOptions},
    meta::MetadataOptions,
    units::Timestamp,
};
use symphonia::default::{get_codecs, get_probe};

use crate::audio::{AudioDecoder, DecoderError, SampleFormat};
struct SymphoniaState {
    reader: Box<dyn FormatReader>,
    decoder: Box<dyn symphonia::core::codecs::audio::AudioDecoder>,
    track_id: u32,
    reached_eof: bool,
}

struct AudioMetadata {
    sample_count: u64,
    sample_rate: u32,
    channel_count: usize,
    frame_count: u64,
    bytes_per_frame: usize,
}

#[derive(Default)]
struct AudioFrameBuffer {
    buffer: Vec<u8>,
    frame_count: usize,
    frame_offset: Option<usize>,
}

impl AudioFrameBuffer {
    /// Prepare for write pcm frames with `byte_size` bytes, and clear the remaining frame
    fn prepare(&mut self, byte_size: usize) {
        if self.buffer.len() < byte_size {
            self.buffer = vec![0u8; byte_size];
        }

        self.clear()
    }

    /// Clear remaining frame
    fn clear(&mut self) {
        self.frame_count = 0;
        self.frame_offset = None;
    }

    // fn has_remaining(&self) -> bool {
    //     self.frame_offset.is_some()
    // }
}

pub struct SymphoniaDecoder {
    sample_format: SampleFormat,
    metadata: AudioMetadata,
    symphonia_state: SymphoniaState,
    frame_position: AtomicI64,
    frame_buffer: AudioFrameBuffer,
    mutex: Mutex<()>,
}

impl SymphoniaDecoder {
    pub fn from_io<T>(mut stream: T, sample_format: SampleFormat) -> Result<Self, DecoderError>
    where
        T: Read + Seek + Send + Sync + 'static,
    {
        // Measure stream length
        let length = stream.seek(io::SeekFrom::End(0))?;

        let adapter = MediaSourceAdapter {
            inner: stream,
            length: Some(length),
        };

        // Construct media source stream
        let mss = MediaSourceStream::new(Box::new(adapter), MediaSourceStreamOptions::default());
        Self::from_media_source_stream(mss, sample_format)
    }

    pub fn from_memory<T>(data: T, sample_format: SampleFormat) -> Result<Self, DecoderError>
    where
        T: AsRef<[u8]> + Send + Sync + 'static,
    {
        Self::from_io(Cursor::new(data), sample_format)
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
        let frame_count = track.num_frames.ok_or(DecoderError::UnsupportedFormat)?;
        let sample_count = frame_count * channel_count as u64;

        let decoder =
            get_codecs().make_audio_decoder(codec_params, &AudioDecoderOptions::default())?;

        let bytes_per_frame = sample_format.byte_size() * channel_count;

        let frame_buffer_frames = codec_params.max_frames_per_packet.unwrap_or(4096) as usize;
        let frame_buffer_size = frame_buffer_frames * bytes_per_frame;

        Ok(Self {
            sample_format,
            metadata: AudioMetadata {
                sample_count,
                sample_rate,
                channel_count,
                frame_count,
                bytes_per_frame,
            },
            frame_position: AtomicI64::new(0),
            mutex: Mutex::new(()),
            symphonia_state: SymphoniaState {
                reader: format,
                decoder,
                track_id,
                reached_eof: false,
            },

            frame_buffer: AudioFrameBuffer {
                buffer: vec![0u8; frame_buffer_size],
                frame_offset: None,
                frame_count: 0,
            },
        })
    }
}

impl AudioDecoder for SymphoniaDecoder {
    fn get_sample_format(&self) -> SampleFormat {
        self.sample_format
    }

    fn get_sample_count(&self) -> u64 {
        self.metadata.sample_count
    }

    fn get_sample_rate(&self) -> u32 {
        self.metadata.sample_rate
    }

    fn get_channel_count(&self) -> usize {
        self.metadata.channel_count
    }

    fn get_frame_count(&self) -> u64 {
        self.metadata.frame_count
    }

    fn tell(&self) -> i64 {
        self.frame_position.load(Ordering::Relaxed)
    }

    fn seek(&mut self, position: i64) -> Result<(), DecoderError> {
        let _guard = self.mutex.lock();

        // Argument range check
        if position < 0 {
            return Err(DecoderError::IOError(io::Error::new(
                io::ErrorKind::InvalidInput,
                "negative seek position is invalid",
            )));
        }

        let position = position as u64;
        if position >= self.metadata.frame_count {
            return Err(DecoderError::IOError(io::Error::new(
                io::ErrorKind::InvalidInput,
                "seek position exceeds total frame count",
            )));
        }

        let current_position = self.frame_position.load(Ordering::Relaxed) as u64;
        if position == current_position {
            return Ok(());
        }

        // Clear frame buffer
        self.frame_buffer.clear();
        self.symphonia_state.reached_eof = false;

        // Reset decoder to ensure clean state after seek
        self.symphonia_state.decoder.reset();

        // 1. Seek the reader to the nearest packet (before or at the target frame)
        let seeked_to = self
            .symphonia_state
            .reader
            .seek(
                SeekMode::Accurate,
                SeekTo::Timestamp {
                    ts: Timestamp::new(position as i64),
                    track_id: self.symphonia_state.track_id,
                },
            )
            .map_err(|_| {
                DecoderError::IOError(io::Error::new(io::ErrorKind::Other, "Failed to seek"))
            })?;

        // 2. Calculate how many frames to skip to reach the exact target frame.
        //    With SeekMode::Accurate, actual_ts <= required_ts, so delta >= 0.
        let delta = seeked_to
            .required_ts
            .checked_delta(seeked_to.actual_ts)
            .unwrap_or_default();
        let mut skip_frames = delta.get() as usize;

        // 3. Decode packets and skip frames until we reach the exact target position.
        //    This leverages AudioFrameBuffer + frame_offset: any excess frames beyond
        //    the target are left in the buffer for subsequent read() calls.
        while skip_frames > 0 {
            match self.symphonia_state.reader.next_packet() {
                Ok(Some(packet)) => match self.symphonia_state.decoder.decode(&packet) {
                    Ok(decoded) => {
                        let decoded_frames = decoded.frames();
                        if decoded_frames == 0 {
                            continue;
                        }

                        let frame_byte_size = self.metadata.bytes_per_frame;
                        let decoded_byte_size = decoded_frames * frame_byte_size;

                        if decoded_frames <= skip_frames {
                            // Entire packet falls within the skip range — discard it
                            skip_frames -= decoded_frames;
                        } else {
                            // This packet contains the target frame. Buffer the decoded
                            // audio and set frame_offset so that the next read() starts
                            // from the exact target frame within this packet.
                            let consumed_frames = skip_frames;
                            self.frame_buffer.prepare(decoded_byte_size);
                            copy_interleaved_frames_as(
                                decoded,
                                &mut self.frame_buffer.buffer[..decoded_byte_size],
                                self.sample_format,
                            );

                            self.frame_buffer.frame_count = decoded_frames;
                            self.frame_buffer.frame_offset = Some(consumed_frames);
                            skip_frames = 0;
                        }
                    }
                    Err(_) => continue,
                },
                Ok(None) => break,
                Err(SymphoniaError::ResetRequired) => {
                    self.symphonia_state.decoder.reset();
                    continue;
                }
                Err(_) => break,
            }
        }

        self.frame_position
            .store(position as i64, Ordering::Relaxed);
        Ok(())
    }

    fn read(&mut self, buffer: &mut [u8], frame_count: u32) -> Result<u32, DecoderError> {
        let _guard = self.mutex.lock();

        let mut position = 0usize;
        let mut required_frames = frame_count as usize;

        // Bytes each frame (sample size * channel count)
        let frame_byte_size = self.metadata.bytes_per_frame;

        // Check remaining frames from the last read
        if let Some(frame_offset) = self.frame_buffer.frame_offset {
            let available_frames = self.frame_buffer.frame_count - frame_offset;

            let take_frames = min(required_frames as usize, available_frames);
            let take_frame_bytes = take_frames * frame_byte_size;

            let buffer_begin = frame_offset * frame_byte_size;
            buffer[position..position + take_frame_bytes].copy_from_slice(
                &self.frame_buffer.buffer[buffer_begin..buffer_begin + take_frame_bytes],
            );

            position += take_frame_bytes;
            required_frames -= take_frames;

            let new_offset = frame_offset + take_frames;

            if new_offset == self.frame_buffer.frame_count {
                // All the frames in the buffer has been used
                self.frame_buffer.frame_offset = None;
                self.frame_buffer.frame_count = 0;
            } else {
                // Some remaining frames
                // In this branch, `required_frames` should be zero
                assert_eq!(required_frames, 0);
                self.frame_buffer.frame_offset = Some(new_offset)
            }
        }

        // Decode new frames
        while required_frames > 0 {
            // Decode packet
            let packet = match self.symphonia_state.reader.next_packet() {
                Ok(Some(packet)) => packet,
                Ok(None) => {
                    self.symphonia_state.reached_eof = true;
                    break;
                }
                Err(SymphoniaError::ResetRequired) => {
                    self.symphonia_state.decoder.reset();
                    continue;
                }
                Err(err) => return Err(err.into()),
            };

            let decoded = self.symphonia_state.decoder.decode(&packet)?;
            let decoded_frames = decoded.frames();
            if decoded.is_empty() {
                continue;
            }

            let decoded_byte_size = decoded_frames * frame_byte_size;
            if decoded_frames <= required_frames {
                // If the count of decoded frames less than required frames,
                // copy them into output buffer instead of internal frame buffer
                let new_frames_byte_size = decoded_byte_size;
                copy_interleaved_frames_as(
                    decoded,
                    &mut buffer[position..(position + new_frames_byte_size)],
                    self.sample_format,
                );

                position += new_frames_byte_size;
                required_frames -= decoded_frames;
            } else {
                // Required frames less then decoded frames,
                // remain some frames, copy all the decoded frames to buffer firstly
                self.frame_buffer.prepare(decoded_byte_size);
                copy_interleaved_frames_as(
                    decoded,
                    &mut self.frame_buffer.buffer[..decoded_byte_size],
                    self.sample_format,
                );

                self.frame_buffer.frame_count = decoded_frames;

                // Copy required frames
                let new_frames_byte_size = required_frames * frame_byte_size;
                buffer[position..(position + new_frames_byte_size)]
                    .copy_from_slice(&self.frame_buffer.buffer[0..new_frames_byte_size]);
                self.frame_buffer.frame_offset = Some(required_frames);
                position += new_frames_byte_size;
                required_frames = 0;
            }
        }

        // Return read frame count
        let read_frames = frame_count as usize - required_frames;
        self.frame_position
            .fetch_add(read_frames as i64, Ordering::Relaxed);

        Ok(read_frames as u32)
    }
}

fn copy_interleaved_frames_as(
    audio_buffer: GenericAudioBufferRef<'_>,
    dst: impl AsMut<[u8]>,
    sample_format: SampleFormat,
) {
    match sample_format {
        SampleFormat::Float32 => {
            audio_buffer.copy_bytes_interleaved_as::<f32, _>(dst);
        }
        SampleFormat::Int16 => {
            audio_buffer.copy_bytes_interleaved_as::<i16, _>(dst);
        }
    }
}

struct MediaSourceAdapter<T> {
    inner: T,
    length: Option<u64>,
}

impl<T: Read> Read for MediaSourceAdapter<T> {
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        self.inner.read(buf)
    }
}

impl<T: Seek> Seek for MediaSourceAdapter<T> {
    fn seek(&mut self, pos: io::SeekFrom) -> io::Result<u64> {
        self.inner.seek(pos)
    }
}

impl<T: Read + Seek + Send + Sync + 'static> MediaSource for MediaSourceAdapter<T> {
    fn is_seekable(&self) -> bool {
        true
    }

    fn byte_len(&self) -> Option<u64> {
        self.length
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
