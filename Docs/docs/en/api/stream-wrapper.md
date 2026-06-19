# StreamWrapper

`Uniasset.StreamWrapper` is the built-in `IUniassetStream` implementation for adapting `System.IO.Stream`.

## Definition

```csharp
namespace Uniasset
{
    public sealed class StreamWrapper : IUniassetStream
    {
        public StreamWrapper(Stream stream);
        public int Read(Span<byte> buffer);
        public long Seek(long offset, SeekOrigin origin);
    }
}
```

## Constructor

```csharp
public StreamWrapper(Stream stream)
```

| Parameter | Type | Description |
|------|------|------|
| `stream` | `Stream` | The stream to wrap; it must be readable and seekable |

If `stream` is `null`, the constructor throws `ArgumentNullException`. If it is not readable or not seekable, it throws `ArgumentException`.

## Methods

### `Read`

```csharp
public int Read(Span<byte> buffer)
```

Reads data from the underlying `Stream`.

### `Seek`

```csharp
public long Seek(long offset, SeekOrigin origin)
```

Maps Uniasset's `SeekOrigin` to `System.IO.SeekOrigin` and performs the seek on the underlying stream.

## Examples

### Wrap a `Stream` manually

```csharp
using Uniasset;
using Uniasset.Image;

using var fileStream = File.OpenRead("photo.png");
var wrapper = new StreamWrapper(fileStream);

using var image = new ImageAsset();
image.LoadIO(wrapper);
```

### Use with `AudioAsset`

```csharp
using Uniasset;
using Uniasset.Audio;

using var fileStream = File.OpenRead("music.flac");
var wrapper = new StreamWrapper(fileStream);

using var audio = new AudioAsset();
audio.LoadIO(wrapper);
```

!!! tip "Convenience overload on ImageAsset"
    `ImageAsset.LoadIO(Stream)` creates `StreamWrapper` internally, so you can pass `System.IO.Stream` directly when loading images:

    ```csharp
    using var fileStream = File.OpenRead("photo.png");
    image.LoadIO(fileStream);
    ```

## Good fit when

- You already have a `FileStream`, `MemoryStream`, or another .NET `Stream`
- You want to reuse existing stream logic instead of writing `IUniassetStream`
- You need to connect a standard .NET stream to `AudioAsset.LoadIO(...)`
