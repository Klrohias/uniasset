# ImageAsset

`Uniasset.Image.ImageAsset` is the high-level image wrapper for loading, cropping, resizing, cloning, and converting image data into Unity `Texture2D` objects.

## Definition

```csharp
namespace Uniasset.Image
{
    public sealed class ImageAsset : IDisposable, ICloneable { }
}
```

## Constructor

```csharp
public ImageAsset()
```

Creates an empty `ImageAsset` instance.

## Read-only properties

| Property | Type | Description |
|------|------|------|
| `Width` | `int` | Current image width |
| `Height` | `int` | Current image height |
| `ChannelCount` | `int` | Number of channels in the current image |
| `PixelType` | `PixelType` | Current pixel layout |

Common `ChannelCount` values:

- `3`: RGB
- `4`: RGBA

## Loading

### `Load`

```csharp
public void Load(string path, int expectedWidth = 0, int expectedHeight = 0)
```

Loads an image from a file path.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `path` | `string` | — | Image file path |
| `expectedWidth` | `int` | `0` | Requested decoded width; `0` keeps the original width |
| `expectedHeight` | `int` | `0` | Requested decoded height; `0` keeps the original height |

```csharp
var image = new ImageAsset();
image.Load("photo.png");
image.Load("photo.png", 256, 256);
```

Negative `expectedWidth` or `expectedHeight` values throw `ArgumentOutOfRangeException`.

### `Load(Span<byte>)`

```csharp
public void Load(Span<byte> data, int expectedWidth = 0, int expectedHeight = 0)
```

Loads from encoded image bytes in memory.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `data` | `Span<byte>` | — | Encoded image bytes |
| `expectedWidth` | `int` | `0` | Requested decoded width |
| `expectedHeight` | `int` | `0` | Requested decoded height |

```csharp
byte[] imageData = File.ReadAllBytes("photo.png");
image.Load(imageData);
```

### `LoadAsync`

```csharp
public Task LoadAsync(string path, int expectedWidth = 0, int expectedHeight = 0)
```

Asynchronously loads an image from a file path.

```csharp
await image.LoadAsync("photo.png");
```

### `LoadAsync(byte[])`

```csharp
public Task LoadAsync(byte[] data, int expectedWidth = 0, int expectedHeight = 0)
```

Asynchronously loads from `byte[]`.

```csharp
byte[] data = File.ReadAllBytes("photo.png");
await image.LoadAsync(data);
```

### `LoadIO`

```csharp
public void LoadIO(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
```

Loads from `IUniassetStream`.

### `LoadIO(Stream)`

```csharp
public void LoadIO(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
```

Loads from `System.IO.Stream`. Internally, Uniasset wraps it with [`StreamWrapper`](stream-wrapper.md).

### `LoadIOAsync`

```csharp
public Task LoadIOAsync(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
public Task LoadIOAsync(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
```

Asynchronously loads from a stream.

## Lifetime

### `Unload`

```csharp
public void Unload()
```

Unloads the currently decoded image while keeping the `ImageAsset` instance reusable.

### `Dispose`

```csharp
public void Dispose()
```

Releases the underlying native resources. The instance should not be used afterward.

## Image processing

### `Crop`

```csharp
public void Crop(int x, int y, int width, int height)
```

Crops the image in place.

| Parameter | Type | Description |
|------|------|------|
| `x` | `int` | Left boundary |
| `y` | `int` | Top boundary |
| `width` | `int` | Crop width |
| `height` | `int` | Crop height |

```csharp
image.Crop(10, 10, 100, 100);
```

### `CropAsync`

```csharp
public Task CropAsync(int x, int y, int width, int height)
```

Asynchronously crops the image.

### `CropMultiple`

```csharp
public ImageAsset[] CropMultiple(CropOptions[] optionsArray)
```

Batch-crops the image and returns multiple new `ImageAsset` instances.

| Parameter | Type | Description |
|------|------|------|
| `optionsArray` | `CropOptions[]` | Crop rectangle array |

The returned array preserves the same order as the input.

```csharp
var crops = new[]
{
    new CropOptions(0, 0, 50, 50),
    new CropOptions(50, 0, 50, 50),
};
ImageAsset[] results = image.CropMultiple(crops);
```

### `CropMultipleAsync`

```csharp
public Task<ImageAsset[]> CropMultipleAsync(CropOptions[] optionsArray)
```

Asynchronously batch-crops the image.

### `Resize`

```csharp
public void Resize(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
```

Resizes the image in place.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `width` | `int` | — | Target width |
| `height` | `int` | — | Target height |
| `filter` | `ResizeFilter` | `Nearest` | Resize filter algorithm |

```csharp
image.Resize(256, 256);
image.Resize(256, 256, ResizeFilter.Lanczos3);
```

### `ResizeAsync`

```csharp
public Task ResizeAsync(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
```

Asynchronously resizes the image.

## Conversion

### `ToTexture2D`

```csharp
public Texture2D ToTexture2D(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
)
```

Converts the current image into a Unity `Texture2D`.

Only images with `ChannelCount` equal to `3` or `4` are supported; otherwise `NotSupportedException` is thrown.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `mipmap` | `bool` | `false` | Whether to generate mipmaps |
| `linear` | `bool` | `true` | Whether to use linear color space |
| `noLongerReadable` | `bool` | `true` | Whether to mark the texture unreadable after upload |

```csharp
Texture2D texture = image.ToTexture2D();
```

### `ToTexture2DAsync`

```csharp
public Task<Texture2D> ToTexture2DAsync(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
)
```

Asynchronously converts the image to `Texture2D`.
