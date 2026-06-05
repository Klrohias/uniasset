# IUniassetStream

`Uniasset.IUniassetStream` 是一个接口，用于抽象流式 I/O 操作，允许从任意数据源加载资源。

## 接口定义

```csharp
namespace Uniasset
{
    public interface IUniassetStream
    {
        int Read(Span<byte> buffer);
        long Seek(long offset, SeekOrigin origin);
    }
}
```

## 方法

### Read

```csharp
int Read(Span<byte> buffer);
```

从流中读取数据到缓冲区。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `buffer` | `Span<byte>` | 目标缓冲区 |

**返回值：** 实际读取的字节数。返回 `0` 表示已到达流末尾。

---

### Seek

```csharp
long Seek(long offset, SeekOrigin origin);
```

移动流的读取位置。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `offset` | `long` | 偏移量 |
| `origin` | `SeekOrigin` | 偏移起点 |

**返回值：** 新的流位置。

---

## SeekOrigin

```csharp
namespace Uniasset
{
    public enum SeekOrigin
    {
        Begin = 0,
        Current = 1,
        End = 2
    }
}
```

| 成员 | 值 | 说明 |
|------|:--:|------|
| `Begin` | 0 | 从流的开头计算偏移 |
| `Current` | 1 | 从当前位置计算偏移 |
| `End` | 2 | 从流的末尾计算偏移 |

---

## StreamWrapper

`Uniasset.StreamWrapper` 是 `IUniassetStream` 的内置实现，用于将 `System.IO.Stream` 适配为 `IUniassetStream`。

### 类定义

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

### 构造函数

```csharp
public StreamWrapper(Stream stream)
```

创建一个新的 `StreamWrapper` 实例。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `stream` | `Stream` | 要包装的流，**必须可读且可寻址** |

### 使用示例

```csharp
using Uniasset;
using Uniasset.Image;

// 从 FileStream 加载
using var fileStream = File.OpenRead("photo.png");
var wrapper = new StreamWrapper(fileStream);

var image = new ImageAsset();
image.LoadIO(wrapper);
```

!!! tip "便捷方式"
    `ImageAsset.LoadIO(Stream)` 方法内部会自动创建 `StreamWrapper`，无需手动包装：

    ```csharp
    using var fileStream = File.OpenRead("photo.png");
    image.LoadIO(fileStream);  // 自动包装
    ```

---

## 自定义实现

你可以实现 `IUniassetStream` 接口来从任意数据源加载资源：

### 从内存加载

```csharp
public class MemoryStreamAdapter : IUniassetStream
{
    private readonly byte[] _data;
    private int _position;

    public MemoryStreamAdapter(byte[] data)
    {
        _data = data;
    }

    public int Read(Span<byte> buffer)
    {
        int available = _data.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
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
        return _position;
    }
}

// 使用
var adapter = new MemoryStreamAdapter(imageBytes);
image.LoadIO(adapter);
```

### 从网络加载

```csharp
public class HttpStreamAdapter : IUniassetStream
{
    private readonly HttpClient _client = new();
    private byte[] _buffer;
    private int _position;

    public HttpStreamAdapter(string url)
    {
        _buffer = _client.GetByteArrayAsync(url).GetAwaiter().GetResult();
    }

    public int Read(Span<byte> buffer)
    {
        int available = _buffer.Length - _position;
        int toRead = Math.Min(buffer.Length, available);
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
        return _position;
    }
}
```

## 应用场景

| 场景 | 说明 |
|------|------|
| 内存中的数据 | 已有 `byte[]` 数据，不想写入临时文件 |
| 网络资源 | 从 HTTP/FTP 等协议加载 |
| 自定义文件格式 | 资源嵌入在自定义打包格式中 |
| 加密/解密 | 需要实时解密的数据流 |
| 测试 | 使用 Mock 流进行单元测试 |
