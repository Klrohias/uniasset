# StreamWrapper

`Uniasset.StreamWrapper` 是 `IUniassetStream` 的内置实现，用于将 `System.IO.Stream` 适配为 `IUniassetStream`。

## 类定义

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

## 构造函数

```csharp
public StreamWrapper(Stream stream)
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `stream` | `Stream` | 要包装的流，必须可读且可定位 |

当 `stream` 为 `null` 时会抛出 `ArgumentNullException`；不可读或不可定位时会抛出 `ArgumentException`。

## 方法

### Read

```csharp
public int Read(Span<byte> buffer)
```

从底层 `Stream` 读取数据。

### Seek

```csharp
public long Seek(long offset, SeekOrigin origin)
```

将 Uniasset 的 `SeekOrigin` 转换为 `System.IO.SeekOrigin` 并执行定位。

## 使用示例

### 手动包装 Stream

```csharp
using Uniasset;
using Uniasset.Image;

using var fileStream = File.OpenRead("photo.png");
var wrapper = new StreamWrapper(fileStream);

using var image = new ImageAsset();
image.LoadIO(wrapper);
```

### 配合 AudioAsset 使用

```csharp
using Uniasset;
using Uniasset.Audio;

using var fileStream = File.OpenRead("music.flac");
var wrapper = new StreamWrapper(fileStream);

using var audio = new AudioAsset();
audio.LoadIO(wrapper);
```

!!! tip "ImageAsset 的便捷重载"
    `ImageAsset.LoadIO(Stream)` 会在内部自动创建 `StreamWrapper`，因此图片加载时可以直接传入 `System.IO.Stream`：

    ```csharp
    using var fileStream = File.OpenRead("photo.png");
    image.LoadIO(fileStream);
    ```

## 适用场景

- 你已经拿到一个 `FileStream`、`MemoryStream` 或其他 .NET `Stream`
- 希望复用现有 `Stream` 逻辑，而不是手写 `IUniassetStream`
- 需要把标准 .NET 流接入 `AudioAsset.LoadIO(...)`
