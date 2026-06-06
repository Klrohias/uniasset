# IUniassetStream

`Uniasset.IUniassetStream` 用于抽象可读取、可定位的数据源。`ImageAsset.LoadIO(...)` 和 `AudioAsset.LoadIO(...)` 都依赖该接口。

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

| 参数 | 类型 | 说明 |
|------|------|------|
| `buffer` | `Span<byte>` | 目标缓冲区 |

返回实际读取的字节数；返回 `0` 表示已到达流末尾。

### Seek

```csharp
long Seek(long offset, SeekOrigin origin);
```

移动流的读取位置。

| 参数 | 类型 | 说明 |
|------|------|------|
| `offset` | `long` | 偏移量 |
| `origin` | `SeekOrigin` | 偏移起点 |

返回新的流位置。

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
|------|----|------|
| `Begin` | 0 | 从流开头计算偏移 |
| `Current` | 1 | 从当前位置计算偏移 |
| `End` | 2 | 从流末尾计算偏移 |

## 自定义实现

你可以实现 `IUniassetStream` 来接入内存数据、打包资源或其他自定义存储：

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
```

## 说明

- `ImageAsset` 同时提供 `LoadIO(IUniassetStream)` 和 `LoadIO(Stream)`。
- `AudioAsset` 只提供 `LoadIO(IUniassetStream)`。
- 自定义实现必须同时支持顺序读取和随机定位。
