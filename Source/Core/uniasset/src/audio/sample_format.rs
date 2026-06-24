#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum SampleFormat {
    Float32 = 0,
    Int16 = 1,
}

impl SampleFormat {
    pub fn byte_size(&self) -> usize {
        match self {
            SampleFormat::Float32 => 4,
            SampleFormat::Int16 => 2,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn float32_byte_size() {
        assert_eq!(SampleFormat::Float32.byte_size(), 4);
    }

    #[test]
    fn int16_byte_size() {
        assert_eq!(SampleFormat::Int16.byte_size(), 2);
    }

    #[test]
    fn sample_format_copy_clone() {
        let fmt = SampleFormat::Float32;
        let fmt2 = fmt;
        assert_eq!(fmt, fmt2);
        assert_eq!(fmt.clone(), fmt2);
    }
}
