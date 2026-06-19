# Image Loading Guide

This guide covers how to use `Uniasset.Image.ImageAsset` to load, process, and convert image assets.

## Supported formats

Uniasset currently supports:

| Format | Extensions | Notes |
|------|--------|------|
| JPEG | `.jpg`, `.jpeg` | Lossy compression, good for photos |
| WebP | `.webp` | Supports lossy and lossless compression |
| PNG | `.png` | Lossless compression with alpha |
| BMP | `.bmp` | Broad compatibility |
| TGA | `.tga` | Common in game pipelines |
| PSD | `.psd` | Photoshop source files |

## Basic loading

### Load from file

```csharp
using Uniasset.Image;

using var image = new ImageAsset();
image.Load("Assets/Images/photo.png");

Debug.Log($"Image size: {image.Width} x {image.Height}");
Debug.Log($"Channels: {image.ChannelCount}");
```

### Load from a byte array

```csharp
using var image = new ImageAsset();

byte[] data = File.ReadAllBytes("photo.png");
image.Load(data);
```

### Load from a stream

```csharp
using Uniasset.Image;

using var image = new ImageAsset();
using var stream = File.OpenRead("photo.png");

image.LoadIO(stream);
```

### Load asynchronously

```csharp
using var image = new ImageAsset();

await image.LoadAsync("photo.png");
```

!!! note
    Asynchronous loading currently exists for `LoadAsync(string)` and `LoadAsync(byte[])`. `LoadIO(...)` has no async overload in this guide's scope.

## Decode directly to a target size

You can pass `expectedWidth` and `expectedHeight` during decode to reduce later memory and processing overhead:

```csharp
image.Load("large_photo.png", expectedWidth: 256, expectedHeight: 256);
```

If you do not need resize-on-decode, pass `0`:

```csharp
image.Load("photo.png", expectedWidth: 0, expectedHeight: 0);
```

!!! warning
    `expectedWidth` and `expectedHeight` must not be negative, or `ArgumentOutOfRangeException` will be thrown.

## Image processing

### Crop

```csharp
image.Crop(x: 100, y: 100, width: 200, height: 200);
```

### Crop asynchronously

```csharp
await image.CropAsync(100, 100, 200, 200);
```

### Batch crop

Useful for extracting multiple sprites from an atlas:

```csharp
var crops = new CropOptions[]
{
    new CropOptions(0, 0, 32, 32),
    new CropOptions(32, 0, 32, 32),
    new CropOptions(64, 0, 32, 32),
    new CropOptions(96, 0, 32, 32),
};

ImageAsset[] frames = image.CropMultiple(crops);
```

### Batch crop asynchronously

```csharp
ImageAsset[] frames = await image.CropMultipleAsync(crops);
```

### Resize

```csharp
image.Resize(256, 256);
```

### Resize asynchronously

```csharp
await image.ResizeAsync(256, 256);
```

## Convert to `Texture2D`

### Synchronous conversion

```csharp
Texture2D texture = image.ToTexture2D();
```

### Asynchronous conversion

```csharp
Texture2D texture = await image.ToTexture2DAsync();
```

### Parameters

```csharp
Texture2D ToTexture2D(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
);
```

| Parameter | Description |
|------|------|
| `mipmap` | Whether to generate mipmaps |
| `linear` | Whether to create the texture in linear color space |
| `noLongerReadable` | Whether to make the texture unreadable after upload |

!!! note
    `ToTexture2D()` and `ToTexture2DAsync()` only support image data with `3` or `4` channels.

## Clone

```csharp
ImageAsset copy = image.Clone();
```

This returns a new `ImageAsset` instance that can be processed independently.

## Lifetime management

### Use `using`

```csharp
using var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
```

### Unload loaded data

```csharp
image.Unload();
```

`Unload()` releases the current image content but keeps the `ImageAsset` instance reusable.

## Common use cases

### Load a UI image

```csharp
public class ImageLoader : MonoBehaviour
{
    [SerializeField] private RawImage _rawImage;

    private ImageAsset _imageAsset;

    private async void Start()
    {
        _imageAsset = new ImageAsset();
        await _imageAsset.LoadAsync("Assets/UI/background.png");
        _rawImage.texture = await _imageAsset.ToTexture2DAsync();
    }

    private void OnDestroy()
    {
        _imageAsset?.Dispose();
    }
}
```

### Load and square-crop an avatar

```csharp
public async Task<Texture2D> LoadAvatar(string path, int size)
{
    using var image = new ImageAsset();
    await image.LoadAsync(path);

    int minDim = Math.Min(image.Width, image.Height);
    int x = (image.Width - minDim) / 2;
    int y = (image.Height - minDim) / 2;

    image.Crop(x, y, minDim, minDim);
    image.Resize(size, size);

    return await image.ToTexture2DAsync();
}
```

### Slice a sprite sheet

```csharp
public async Task<Texture2D[]> LoadSpriteSheet(string path, int spriteWidth, int spriteHeight)
{
    using var sheet = new ImageAsset();
    await sheet.LoadAsync(path);

    int cols = sheet.Width / spriteWidth;
    int rows = sheet.Height / spriteHeight;

    var crops = new CropOptions[cols * rows];
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            crops[y * cols + x] = new CropOptions(
                x * spriteWidth,
                y * spriteHeight,
                spriteWidth,
                spriteHeight
            );
        }
    }

    ImageAsset[] sprites = sheet.CropMultiple(crops);
    try
    {
        var textures = new Texture2D[sprites.Length];
        for (int i = 0; i < sprites.Length; i++)
        {
            textures[i] = sprites[i].ToTexture2D();
        }

        return textures;
    }
    finally
    {
        foreach (var sprite in sprites)
        {
            sprite.Dispose();
        }
    }
}
```
