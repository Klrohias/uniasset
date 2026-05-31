const MP3_MAGIC_NUMBER_1: &[u8] = &[0x49, 0x44, 0x33]; // ID3
const MP3_MAGIC_NUMBER_2: &[u8] = &[0xff, 0xfb]; // MPEG frame sync
const OGG_MAGIC_NUMBER: &[u8] = &[
    0x4F, 0x67, 0x67, 0x53, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
];

pub fn is_mp3(data: &[u8]) -> bool {
    data.starts_with(MP3_MAGIC_NUMBER_1) || data.starts_with(MP3_MAGIC_NUMBER_2)
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
