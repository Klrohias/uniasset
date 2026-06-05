# 自定义流指南

本指南介绍如何使用 `IUniassetStream` 接口从自定义数据源加载资源。

## 概述

Uniasset 的 `IUniassetStream` 接口允许你从任意数据源加载图片和音频，而不仅仅是文件或字节数组。这对于以下场景非常有用：

- 从自定义打包格式中读取资源
- 从网络流加载资源
- 读取加密或压缩的数据
- 从内存映射文件读取
- 单元测试中的 Mock 流

## 接口定义

```csharp
public interface IUniassetStream
{
    int Read(Span<byte> buffer);
    long Seek(long offset, SeekOrigin origin);
}
```

### Read 方法

- 从流中读取数据到 `buffer`
- 返回实际读取的字节数
- 返回 `0` 表示已到达流末尾
- `buffer.Length` 是期望读取的最大字节数

### Seek 方法

- 移动流的读取位置
- `offset` 是相对于 `origin` 的偏移量
- 返回新的流位置（从流开头算起）
- `origin` 可以是 `Begin`、`Current` 或 `End`

## 使用 StreamWrapper

对于已有的 `System.IO.Stream`，可以使用内置的 `StreamWrapper` 适配器：

```csharp
using Uniasset;
using Uniasset.Image;

// 方式 1：手动包装
using var fileStream = File.OpenRead("photo.png");
var wrapper = new StreamWrapper(fileStream);
var image = new ImageAsset();
image.LoadIO(wrapper);

// 方式 2：直接传入 Stream（ImageAsset 会自动包装）
using var fileStream = File.OpenRead("photo.png");
image.LoadIO(fileStream);
```

!!! warning "注意事项"
    `StreamWrapper` 要求底层的 `Stream` 必须：
    
    - **可读**：`CanRead` 返回 `true`
    - **可寻址**：`CanSeek` 返回 `true`

## 实现自定义流

### 基本模板

```csharp
public class CustomStream : IUniassetStream
{
    // 你的数据源
    private readonly byte[] _data;
    private int _position;

    public CustomStream(byte[] data)
    {
        _data = data;
    }

    public int Read(Span<byte> buffer)
    {
        // 计算可读取的字节数
        int available = _data.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
        
        if (toRead <= 0) return 0;
        
        // 复制数据
        _data.AsSpan(_position, toRead).CopyTo(buffer);
        _position += toRead;
        
        return toRead;
    }

    public long Seek(long offset, SeekOrigin origin)
    {
        _position = origin switch
        {
            SeekOrigin.Begin => (int)offset,
            SeekOrigin.Current => _position + (int)offset,
            SeekOrigin.End => _data.Length + (int)offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };
        
        // 边界检查
        _position = Math.Clamp(_position, 0, _data.Length);
        
        return _position;
    }
}
```

### 从 Unity AssetBundle 加载

```csharp
public class AssetBundleStream : IUniassetStream
{
    private readonly AssetBundleRequest _request;
    private byte[] _data;
    private int _position;

    public AssetBundleStream(AssetBundle bundle, string assetName)
    {
        var asset = bundle.LoadAsset<TextAsset>(assetName);
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
        _position = origin switch
        {
            SeekOrigin.Begin => (int)offset,
            SeekOrigin.Current => _position + (int)offset,
            SeekOrigin.End => _data.Length + (int)offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };
        _position = Math.Clamp(_position, 0, _data.Length);
        return _position;
    }
}
```

### 从加密数据加载

```csharp
public class EncryptedStream : IUniassetStream
{
    private readonly byte[] _encryptedData;
    private readonly byte[] _key;
    private int _position;

    public EncryptedStream(byte[] encryptedData, byte[] key)
    {
        _encryptedData = encryptedData;
        _key = key;
    }

    public int Read(Span<byte> buffer)
    {
        int available = _encryptedData.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
        if (toRead <= 0) return 0;

        // 简单的 XOR 解密
        for (int i = 0; i < toRead; i++)
        {
            buffer[i] = (byte)(_encryptedData[_position + i] ^ _key[i % _key.Length]);
        }
        
        _position += toRead;
        return toRead;
    }

    public long Seek(long offset, SeekOrigin origin)
    {
        _position = origin switch
        {
            SeekOrigin.Begin => (int)offset,
            SeekOrigin.Current => _position + (int)offset,
            SeekOrigin.End => _encryptedData.Length + (int)offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };
        _position = Math.Clamp(_position, 0, _encryptedData.Length);
        return _position;
    }
}
```

### 从 HTTP 流加载

```csharp
public class HttpStream : IUniassetStream, IDisposable
{
    private readonly HttpClient _client = new();
    private byte[] _buffer;
    private int _position;

    public HttpStream(string url)
    {
        // 预加载整个文件
        // 对于大文件，可以考虑分块加载
        _buffer = _client.GetByteArrayAsync(url).GetAwaiter().GetResult();
    }

    public int Read(Span<byte> buffer)
    {
        int available = _buffer.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
        if (toRead <= 0) return 0;

        _buffer.AsSpan(_position, toRead).CopyTo(buffer);
        _position += toRead;
        return toRead;
    }

    public long Seek(long offset, SeekOrigin origin)
    {
        _position = origin switch
        {
            SeekOrigin.Begin => (int)offset,
            SeekOrigin.Current => _position + (int)offset,
            SeekOrigin.End => _buffer.Length + (int)offset,
            _ => throw new ArgumentOutOfRangeException(nameof(origin))
        };
        _position = Math.Clamp(_position, 0, _buffer.Length);
        return _position;
    }

    public void Dispose()
    {
        _client.Dispose();
    }
}
```

## 使用示例

### 加载图片

```csharp
var stream = new CustomStream(imageBytes);
var image = new ImageAsset();
image.LoadIO(stream);
Texture2D texture = image.ToTexture2D();
image.Dispose();
```

### 加载音频

```csharp
var stream = new CustomStream(audioBytes);
var audio = new AudioAsset();
audio.LoadIO(stream);
AudioClip clip = audio.ToAudioClip();
audio.Dispose();
```

## 最佳实践

### 1. 缓冲区大小

Uniasset 内部会按需调用 `Read` 方法，通常会传入较大的缓冲区。确保你的实现能处理任意大小的缓冲区。

### 2. 边界检查

始终进行边界检查，避免越界访问：

```csharp
public int Read(Span<byte> buffer)
{
    int available = _data.Length - _position;
    int toRead = Math.Min(buffer.Length, available);
    if (toRead <= 0) return 0;  // 到达末尾
    // ...
}
```

### 3. Seek 的正确实现

确保 `Seek` 返回正确的位置值：

```csharp
public long Seek(long offset, SeekOrigin origin)
{
    long newPosition = origin switch
    {
        SeekOrigin.Begin => offset,
        SeekOrigin.Current => _position + offset,
        SeekOrigin.End => _data.Length + offset,
        _ => throw new ArgumentOutOfRangeException(nameof(origin))
    };
    
    // 边界检查
    _position = (int)Math.Clamp(newPosition, 0, _data.Length);
    return _position;
}
```

### 4. 资源释放

如果你的流持有非托管资源，记得实现 `IDisposable`：

```csharp
public class MyStream : IUniassetStream, IDisposable
{
    // ...
    
    public void Dispose()
    {
        // 释放资源
    }
}
```

### 5. 线程安全

Uniasset 可能会在后台线程上调用你的流实现。如果数据源不是线程安全的，考虑添加同步机制。

## 常见问题

### Q: Read 返回 0 后还能继续 Seek 吗？

可以。`Read` 返回 `0` 只是表示当前位置没有更多数据，但你可以通过 `Seek` 移动到其他位置继续读取。

### Q: 需要支持负偏移的 Seek 吗？

`Seek` 的 `offset` 参数可以是负数（例如 `Seek(-10, SeekOrigin.Current)`）。确保你的实现正确处理这种情况。

### Q: 流的长度需要提前知道吗？

不需要。Uniasset 通过 `Seek(0, SeekEnd)` 的返回值来获取流的长度。确保你的实现支持这种用法。
