# Custom Streams Guide

This guide shows how to use `IUniassetStream` to feed image or audio data into Uniasset from arbitrary data sources.

## When to use it

`IUniassetStream` is useful when:

- Assets come from a custom package format
- Data is already in memory and you do not want a temporary file
- Assets need to be decrypted or decompressed before decoding
- Data comes from an `AssetBundle`, database, or network cache
- Tests need a controllable mock data source

## Interface definition

```csharp
public interface IUniassetStream
{
    int Read(Span<byte> buffer);
    long Seek(long offset, SeekOrigin origin);
}
```

### `Read`

```csharp
int Read(Span<byte> buffer);
```

Requirements:

- Fill `buffer` with data
- Return the number of bytes actually read
- Return `0` on EOF

### `Seek`

```csharp
long Seek(long offset, SeekOrigin origin);
```

Requirements:

- Move the current position based on `origin` and `offset`
- Return the new absolute position
- Support `Begin`, `Current`, and `End`

## `SeekOrigin`

```csharp
public enum SeekOrigin
{
    Begin = 0,
    Current = 1,
    End = 2,
}
```

## Working with `ImageAsset` and `AudioAsset`

### Images

`ImageAsset` supports both forms:

```csharp
using var image = new ImageAsset();
image.LoadIO(customStream);
```

Or pass a `System.IO.Stream` directly:

```csharp
using var image = new ImageAsset();
using var fileStream = File.OpenRead("photo.png");

image.LoadIO(fileStream);
```

### Audio

`AudioAsset` only supports `IUniassetStream`:

```csharp
using var audio = new AudioAsset();
audio.LoadIO(customStream);
```

If you start from `System.IO.Stream`, wrap it first:

```csharp
using var audio = new AudioAsset();
using var fileStream = File.OpenRead("music.flac");

var stream = new StreamWrapper(fileStream);
audio.LoadIO(stream);
```

## Using `StreamWrapper`

`StreamWrapper` is the built-in adapter that converts `System.IO.Stream` into `IUniassetStream`.

```csharp
using Uniasset;
using Uniasset.Image;

using var fileStream = File.OpenRead("photo.png");
var wrapper = new StreamWrapper(fileStream);

using var image = new ImageAsset();
image.LoadIO(wrapper);
```

!!! warning "Restrictions"
    `StreamWrapper` requires the underlying `Stream` to be:

    - Readable: `CanRead == true`
    - Seekable: `CanSeek == true`

## Minimal implementation

Here is a minimal `byte[]`-backed implementation:

```csharp
public sealed class MemoryStreamAdapter : IUniassetStream
{
    private readonly byte[] _data;
    private int _position;

    public MemoryStreamAdapter(byte[] data)
    {
        _data = data ?? throw new ArgumentNullException(nameof(data));
    }

    public int Read(Span<byte> buffer)
    {
        int available = _data.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
        if (toRead <= 0) return 0;

        _data.AsSpan(_position, toRead).CopyTo(buffer);
        _position += toRead;
        return toRead;
    }

    public long Seek(long offset, SeekOrigin origin)
    {
        long newPosition = origin switch
        {
            SeekOrigin.Begin => offset,
            SeekOrigin.Current => _position + offset,
            SeekOrigin.End => _data.Length + offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };

        _position = (int)Math.Clamp(newPosition, 0, _data.Length);
        return _position;
    }
}
```

## Usage examples

### Load an image from memory

```csharp
byte[] imageBytes = File.ReadAllBytes("photo.png");
var stream = new MemoryStreamAdapter(imageBytes);

using var image = new ImageAsset();
image.LoadIO(stream);

Texture2D texture = image.ToTexture2D();
```

### Load audio from memory

```csharp
byte[] audioBytes = File.ReadAllBytes("music.mp3");
var stream = new MemoryStreamAdapter(audioBytes);

using var audio = new AudioAsset();
audio.LoadIO(stream);

AudioClip clip = audio.ToAudioClip();
```

### Read from an AssetBundle

```csharp
public sealed class AssetBundleStream : IUniassetStream
{
    private readonly byte[] _data;
    private int _position;

    public AssetBundleStream(AssetBundle bundle, string assetName)
    {
        var asset = bundle.LoadAsset<TextAsset>(assetName);
        if (asset == null) throw new ArgumentException("Asset not found.", nameof(assetName));
        _data = asset.bytes;
    }

    public int Read(Span<byte> buffer)
    {
        int available = _data.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
        if (toRead <= 0) return 0;

        _data.AsSpan(_position, toRead).CopyTo(buffer);
        _position += toRead;
        return toRead;
    }

    public long Seek(long offset, SeekOrigin origin)
    {
        long newPosition = origin switch
        {
            SeekOrigin.Begin => offset,
            SeekOrigin.Current => _position + offset,
            SeekOrigin.End => _data.Length + offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };

        _position = (int)Math.Clamp(newPosition, 0, _data.Length);
        return _position;
    }
}
```

### Read encrypted data

```csharp
public sealed class EncryptedStream : IUniassetStream
{
    private readonly byte[] _encryptedData;
    private readonly byte[] _key;
    private int _position;

    public EncryptedStream(byte[] encryptedData, byte[] key)
    {
        _encryptedData = encryptedData ?? throw new ArgumentNullException(nameof(encryptedData));
        _key = key ?? throw new ArgumentNullException(nameof(key));
    }

    public int Read(Span<byte> buffer)
    {
        int available = _encryptedData.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
        if (toRead <= 0) return 0;

        for (int i = 0; i < toRead; i++)
        {
            buffer[i] = (byte)(_encryptedData[_position + i] ^ _key[(_position + i) % _key.Length]);
        }

        _position += toRead;
        return toRead;
    }

    public long Seek(long offset, SeekOrigin origin)
    {
        long newPosition = origin switch
        {
            SeekOrigin.Begin => offset,
            SeekOrigin.Current => _position + offset,
            SeekOrigin.End => _encryptedData.Length + offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };

        _position = (int)Math.Clamp(newPosition, 0, _encryptedData.Length);
        return _position;
    }
}
```

## Implementation advice

### 1. Always support random access

Do not implement `IUniassetStream` as a one-pass forward-only reader. Uniasset relies on `Seek(...)` to revisit arbitrary positions.

### 2. Handle EOF correctly

Return `0` at EOF. Do not throw and do not return negative values.

### 3. Allow negative offsets

Calls such as `Seek(-10, SeekOrigin.Current)` are valid and should work correctly.

### 4. Return absolute positions

`Seek(...)` should return the new absolute position, not the applied offset.

### 5. Clean up external resources

If your implementation owns file handles, sockets, DB cursors, or other external resources, it should also implement `IDisposable`.

## FAQ

### Can I still `Seek` after `Read` returns `0`?

Yes. `Read == 0` only means the current position is at EOF; it does not invalidate the stream.

### Do I need to know the total length in advance?

Not necessarily, but `Seek(0, SeekOrigin.End)` must still return a valid position, or random-access consumers may fail.

### Can `Read` download data lazily from the network?

Yes, but only if your implementation still preserves `Seek(...)` semantics. In practice, caching to memory or a temporary file is usually simpler.
