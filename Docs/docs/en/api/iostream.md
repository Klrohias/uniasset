# IUniassetStream

`Uniasset.IUniassetStream` abstracts a readable and seekable data source. Both `ImageAsset.LoadIO(...)` and `AudioAsset.LoadIO(...)` depend on it.

## Definition

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

## Methods

### `Read`

```csharp
int Read(Span<byte> buffer);
```

Reads data from the stream into the destination buffer.

| Parameter | Type | Description |
|------|------|------|
| `buffer` | `Span<byte>` | Destination buffer |

Returns the number of bytes actually read. Returning `0` means EOF.

### `Seek`

```csharp
long Seek(long offset, SeekOrigin origin);
```

Moves the read position.

| Parameter | Type | Description |
|------|------|------|
| `offset` | `long` | Offset |
| `origin` | `SeekOrigin` | Origin of the offset |

Returns the new absolute position.

## `SeekOrigin`

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

| Member | Value | Description |
|------|----|------|
| `Begin` | 0 | Offset from the start of the stream |
| `Current` | 1 | Offset from the current position |
| `End` | 2 | Offset from the end of the stream |

## Custom implementations

You can implement `IUniassetStream` for in-memory data, packaged resources, or any custom storage layer:

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

## Notes

- `ImageAsset` provides both `LoadIO(IUniassetStream)` and `LoadIO(Stream)`.
- `AudioAsset` only provides `LoadIO(IUniassetStream)`.
- Custom implementations must support both sequential reads and random access.
