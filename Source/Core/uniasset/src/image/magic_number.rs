const JPEG_MAGIC_NUMBER: &[u8] = &[0xff, 0xd8, 0xff, 0xe0];

pub fn is_webp(data: &[u8]) -> bool {
    data.len() >= 15 && &data[0..4] == b"RIFF"
}

pub fn is_jpeg(data: &[u8]) -> bool {
    data.starts_with(JPEG_MAGIC_NUMBER)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn detect_jpeg() {
        let data = [0xff, 0xd8, 0xff, 0xe0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        assert!(is_jpeg(&data));
    }

    #[test]
    fn detect_webp() {
        let mut data = [0u8; 16];
        data[0..4].copy_from_slice(b"RIFF");
        assert!(is_webp(&data));
    }

    #[test]
    fn not_jpeg() {
        let data = [0xff, 0xd8, 0xff, 0xe1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        assert!(!is_jpeg(&data));
    }

    #[test]
    fn not_webp() {
        let data = [0u8; 16];
        assert!(!is_webp(&data));
    }

    #[test]
    fn webp_too_short() {
        let data = [b'R', b'I', b'F', b'F', 0, 0];
        assert!(!is_webp(&data));
    }
}
