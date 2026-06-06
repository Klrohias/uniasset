# 自定义流指南

本指南介绍如何使用 `IUniassetStream` 接口，把图片或音频数据从任意自定义数据源接入 Uniasset。

## 适用场景

`IUniassetStream` 适合以下场景：

- 资源来自自定义打包格式
- 数据已经在内存中，不想落盘到临时文件
- 资源需要先解密或解压，再交给 Uniasset
- 资源来自 `AssetBundle`、数据库或网络缓存
- 测试时需要可控的 Mock 数据源

## 接口定义

```csharp
public interface IUniassetStream
{
    int Read(Span<byte> buffer);
    long Seek(long offset, SeekOrigin origin);
}
```

### Read

```csharp
int Read(Span<byte> buffer);
```

要求：

- 将数据读取到 `buffer`
- 返回实际读取的字节数
- 到达末尾时返回 `0`

### Seek

```csharp
long Seek(long offset, SeekOrigin origin);
```

要求：

- 根据 `origin` 和 `offset` 移动当前位置
- 返回新的绝对位置
- 必须支持 `Begin`、`Current` 和 `End`

## SeekOrigin

```csharp
public enum SeekOrigin
{
    Begin = 0,
    Current = 1,
    End = 2,
}
```

## 与 ImageAsset / AudioAsset 的配合方式

### 图片

`ImageAsset` 支持两种写法：

```csharp
using var image = new ImageAsset();
image.LoadIO(customStream);
```

或直接传入 `System.IO.Stream`：

```csharp
using var image = new ImageAsset();
using var fileStream = File.OpenRead("photo.png");

image.LoadIO(fileStream);
```

### 音频

`AudioAsset` 只支持 `IUniassetStream`：

```csharp
using var audio = new AudioAsset();
audio.LoadIO(customStream);
```

如果你手里是 `System.IO.Stream`，需要手动包装：

```csharp
using var audio = new AudioAsset();
using var fileStream = File.OpenRead("music.flac");

var stream = new StreamWrapper(fileStream);
audio.LoadIO(stream);
```

## 使用 StreamWrapper

`StreamWrapper` 是内置适配器，用于把 `System.IO.Stream` 转成 `IUniassetStream`。

```csharp
using Uniasset;
using Uniasset.Image;

using var fileStream = File.OpenRead("photo.png");
var wrapper = new StreamWrapper(fileStream);

using var image = new ImageAsset();
image.LoadIO(wrapper);
```

!!! warning "使用限制"
    `StreamWrapper` 要求底层 `Stream` 同时满足：

    - 可读：`CanRead == true`
    - 可定位：`CanSeek == true`

## 最小可用实现

下面是一个基于 `byte[]` 的最小实现模板：

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

## 使用示例

### 从内存加载图片

```csharp
byte[] imageBytes = File.ReadAllBytes("photo.png");
var stream = new MemoryStreamAdapter(imageBytes);

using var image = new ImageAsset();
image.LoadIO(stream);

Texture2D texture = image.ToTexture2D();
```

### 从内存加载音频

```csharp
byte[] audioBytes = File.ReadAllBytes("music.mp3");
var stream = new MemoryStreamAdapter(audioBytes);

using var audio = new AudioAsset();
audio.LoadIO(stream);

AudioClip clip = audio.ToAudioClip();
```

### 从 AssetBundle 中读取

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

### 读取加密数据

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

## 实现建议

### 1. 始终支持随机定位

不要把 `IUniassetStream` 实现成“只能顺序读一次”的接口。Uniasset 依赖 `Seek(...)` 在不同位置反复读取数据。

### 2. 正确处理 EOF

读到末尾时返回 `0`，不要抛异常，也不要返回负数。

### 3. 允许负偏移

`Seek(-10, SeekOrigin.Current)` 这样的调用是合法的，实现时要正确处理。

### 4. 返回绝对位置

`Seek(...)` 的返回值应是新的绝对位置，而不是偏移量。

### 5. 注意资源释放

如果你的实现内部持有文件句柄、Socket、数据库游标或其他外部资源，应额外实现 `IDisposable`。

## 常见问题

### `Read` 返回 0 后还能继续 `Seek` 吗？

可以。`Read` 返回 `0` 只表示当前位置已经没有更多数据，并不意味着流失效。

### 流长度必须提前知道吗？

不一定，但你的 `Seek(0, SeekOrigin.End)` 必须能够返回一个有效位置，否则依赖随机定位的读取逻辑会出问题。

### 可以在 `Read` 里边读边下载网络数据吗？

可以，但前提是你的实现仍然能满足 `Seek(...)` 的语义。对很多网络源来说，更实际的方式是先缓存到内存或临时文件，再通过 `IUniassetStream` 暴露给 Uniasset。
