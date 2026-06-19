# API Reference

This section documents the current public C# API in `Unity/Uniasset/`. It focuses on the high-level wrappers and excludes `Unsafe` / `Interop` types.

## Namespace overview

| Namespace | Description |
|----------|------|
| `Uniasset` | Core stream interfaces and adapters |
| `Uniasset.Image` | Image-related types |
| `Uniasset.Audio` | Audio-related types |

## Core types

### [`ImageAsset`](image-asset.md)

An image asset wrapper that can load encoded image data from files, memory, or streams, then crop, resize, clone, and convert it to `Texture2D`.

```csharp
using var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
```

### [`AudioAsset`](audio-asset.md)

An audio asset wrapper that can load audio from files, memory, or custom streams, then expose PCM reads, seeking, and `AudioClip` conversion.

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
AudioClip clip = audio.ToAudioClip();
```

### [`CropOptions`](crop-options.md)

A struct that describes a crop rectangle for batch cropping.

```csharp
var crops = new CropOptions[]
{
    new CropOptions(0, 0, 100, 100),
    new CropOptions(100, 0, 100, 100),
};
ImageAsset[] results = image.CropMultiple(crops);
```

### [`ResizeFilter`](resize-filter.md)

An enum that selects the resize filter, including `Nearest`, `Box`, `Lanczos3`, and `Gaussian`.

```csharp
image.Resize(256, 256, ResizeFilter.Lanczos3);
```

### [`PixelType`](pixel-type.md)

An enum describing the image pixel layout.

```csharp
if (image.PixelType == PixelType.RGBA) { ... }
```

### [`SampleFormat`](sample-format.md)

An enum describing the sample format used during audio decode, supporting `Float` and `Int16`.

### [`IUniassetStream`](iostream.md)

The custom stream interface.

```csharp
public interface IUniassetStream
{
    int Read(Span<byte> buffer);
    long Seek(long offset, SeekOrigin origin);
}
```

### [`StreamWrapper`](stream-wrapper.md)

The adapter from `System.IO.Stream` to `IUniassetStream`.

## Usage patterns

### Synchronous loading

```csharp
using var asset = new ImageAsset();
asset.Load("path/to/file.png");
```

### Asynchronous loading

```csharp
using var asset = new ImageAsset();
await asset.LoadAsync("path/to/file.png");
```

### Releasing resources

`ImageAsset` and `AudioAsset` both implement `IDisposable` and should be released promptly:

```csharp
using var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
```

## Type list

| Type | Namespace | Description |
|------|----------|------|
| `ImageAsset` | `Uniasset.Image` | Image loading, processing, and conversion |
| `CropOptions` | `Uniasset.Image` | Batch crop parameters |
| `ResizeFilter` | `Uniasset.Image` | Resize filter algorithm |
| `PixelType` | `Uniasset.Image` | Pixel format |
| `AudioAsset` | `Uniasset.Audio` | Audio loading, reading, and conversion |
| `SampleFormat` | `Uniasset.Audio` | Audio sample format |
| `IUniassetStream` | `Uniasset` | Custom stream interface |
| `SeekOrigin` | `Uniasset` | Stream seek reference |
| `StreamWrapper` | `Uniasset` | `System.IO.Stream` adapter |
