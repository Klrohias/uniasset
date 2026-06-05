const JPEG_MAGIC_NUMBER: &[u8] = &[0xff, 0xd8, 0xff, 0xe0];

pub fn is_webp(data: &[u8]) -> bool {
    data.len() >= 15 && &data[0..4] == b"RIFF"
}

pub fn is_jpeg(data: &[u8]) -> bool {
    data.starts_with(JPEG_MAGIC_NUMBER)
}
