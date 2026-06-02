#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum SampleFormat {
    Float32,
    Int16,
}

impl SampleFormat {
    pub fn byte_size(&self) -> usize {
        match self {
            SampleFormat::Float32 => 4,
            SampleFormat::Int16 => 2,
        }
    }
}
