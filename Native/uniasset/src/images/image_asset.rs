use std::io::Cursor;

use anyhow::{Ok, Result, anyhow};
use image::{ColorType, DynamicImage, GenericImageView, ImageReader};

#[derive(Clone)]
struct ImageDetails {
    size: (u32, u32),
    color: ColorType,
}

#[derive(Default, Clone)]
pub struct ImageAsset {
    dyn_image: Option<DynamicImage>,
    details: Option<ImageDetails>,
}

impl ImageAsset {
    pub fn new() -> Self {
        Default::default()
    }

    fn load(&mut self, dyn_image: DynamicImage) {
        self.dyn_image = Some(dyn_image);
        let dyn_image = self.dyn_image.as_ref().unwrap();
        self.details = Some(ImageDetails {
            size: dyn_image.dimensions(),
            color: dyn_image.color(),
        });
    }

    pub fn unload(&mut self) {
        self.dyn_image = None;
        self.details = None;
    }

    pub fn load_from_file(&mut self, path: &str) -> Result<()> {
        let image = ImageReader::open(path)?;
        self.load(image.decode()?);

        Ok(())
    }

    pub fn load_from_memory(&mut self, bytes: &[u8]) -> Result<()> {
        let image = ImageReader::new(Cursor::new(bytes));
        self.load(image.decode()?);

        Ok(())
    }

    pub fn get_width(&self) -> Result<u32> {
        Ok(self
            .details
            .as_ref()
            .ok_or(anyhow!("Image has not loaded"))?
            .size
            .0)
    }

    pub fn get_height(&self) -> Result<u32> {
        Ok(self
            .details
            .as_ref()
            .ok_or(anyhow!("Image has not loaded"))?
            .size
            .1)
    }

    pub fn resize(&mut self, w: u32, h: u32) -> Result<()> {
        let new_image = self
            .dyn_image
            .as_mut()
            .ok_or(anyhow!("Image has not loaded"))?
            .resize(w, h, image::imageops::FilterType::Nearest);

        self.load(new_image);
        Ok(())
    }

    pub fn crop(&mut self, l: u32, t: u32, w: u32, h: u32) -> Result<()> {
        let new_image = self
            .dyn_image
            .as_mut()
            .ok_or(anyhow!("Image has not loaded"))?
            .crop(l, t, w, h);

        self.load(new_image);
        Ok(())
    }

    pub fn copy_to(&self, dst: &mut [u8]) -> Result<()> {
        let image_ref = self
            .dyn_image
            .as_ref()
            .ok_or(anyhow!("Image has not loaded"))?;

        let bytes = image_ref.as_bytes();
        if bytes.len() != dst.len() {
            return Err(anyhow!("Buffer size mismatch"));
        }

        dst.copy_from_slice(&bytes);

        Ok(())
    }
}
