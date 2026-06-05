use std::io::{self, Read, Seek};

const MP3_MAGIC_NUMBER_1: &[u8] = &[0x49, 0x44, 0x33]; // ID3
const MP3_MAGIC_NUMBER_2: &[u8] = &[0xff, 0xfb]; // MPEG frame sync
const OGG_MAGIC_NUMBER: &[u8] = &[
    0x4F, 0x67, 0x67, 0x53, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
];

const AAC_MAGIC_NUMBER_1: &[u8] = &[0xff, 0xf1];
const AAC_MAGIC_NUMBER_2: &[u8] = &[0xff, 0xf2];

pub fn is_mp3(data: &[u8]) -> bool {
    data.starts_with(MP3_MAGIC_NUMBER_1) || data.starts_with(MP3_MAGIC_NUMBER_2)
}

pub fn is_aac(data: &[u8]) -> bool {
    data.starts_with(AAC_MAGIC_NUMBER_1) || data.starts_with(AAC_MAGIC_NUMBER_2)
}

pub fn is_flac(data: &[u8]) -> bool {
    data.starts_with(b"fLaC")
}

pub fn is_wav(data: &[u8]) -> bool {
    data.len() >= 12 && &data[0..4] == b"RIFF" && &data[8..12] == b"WAVE"
}

pub fn is_ogg(data: &[u8]) -> bool {
    data.len() >= 14 && data.starts_with(OGG_MAGIC_NUMBER)
}

#[derive(Debug, PartialEq, Eq)]
pub enum AudioFormatProbe {
    Mp3,
    Flac,
    Wav,
    OggVorbis,
    Aac,
    Unsupported,
}

pub fn probe_format(data: impl AsRef<[u8]>) -> AudioFormatProbe {
    if is_wav(data.as_ref()) {
        AudioFormatProbe::Wav
    } else if is_flac(data.as_ref()) {
        AudioFormatProbe::Flac
    } else if is_ogg(data.as_ref()) {
        AudioFormatProbe::OggVorbis
    } else if is_mp3(data.as_ref()) {
        AudioFormatProbe::Mp3
    } else if is_aac(data.as_ref()) {
        AudioFormatProbe::Aac
    } else {
        AudioFormatProbe::Unsupported
    }
}

pub fn probe_format_from_stream(
    mut stream: impl Read + Seek,
) -> Result<AudioFormatProbe, io::Error> {
    let mut magic_number_buffer = [0u8; 16];
    stream.read_exact(&mut magic_number_buffer)?;
    stream.seek(io::SeekFrom::Start(0))?;

    if is_wav(&magic_number_buffer) {
        Ok(AudioFormatProbe::Wav)
    } else if is_flac(&magic_number_buffer) {
        Ok(AudioFormatProbe::Flac)
    } else if is_ogg(&magic_number_buffer) {
        Ok(AudioFormatProbe::OggVorbis)
    } else if is_mp3(&magic_number_buffer) {
        Ok(AudioFormatProbe::Mp3)
    } else if is_aac(&magic_number_buffer) {
        Ok(AudioFormatProbe::Aac)
    } else {
        Ok(AudioFormatProbe::Unsupported)
    }
}
