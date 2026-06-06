# API 参考

本节基于 `Unity/Uniasset/` 中当前公开的 C# API 编写，只包含高层封装，不包含 `Unsafe` / `Interop` 相关类型。

## 命名空间概览

| 命名空间 | 说明 |
|----------|------|
| `Uniasset` | 核心流接口与适配器 |
| `Uniasset.Image` | 图片相关类型 |
| `Uniasset.Audio` | 音频相关类型 |

## 核心类型

### [`ImageAsset`](image-asset.md)

图片资源类，支持从文件、内存或流加载编码后的图片数据，并提供裁剪、缩放、克隆与 `Texture2D` 转换能力。

```csharp
using var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
```

### [`AudioAsset`](audio-asset.md)

音频资源类，支持从文件、内存或自定义流加载音频，并提供 PCM 读取、定位和 `AudioClip` 转换能力。

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
AudioClip clip = audio.ToAudioClip();
```

### [`CropOptions`](crop-options.md)

裁剪区域描述结构体，用于批量裁剪操作。

```csharp
var crops = new CropOptions[]
{
    new CropOptions(0, 0, 100, 100),
    new CropOptions(100, 0, 100, 100),
};
ImageAsset[] results = image.CropMultiple(crops);
```

### [`ResizeFilter`](resize-filter.md)

缩放滤波算法枚举，支持 `Nearest`、`Box`、`Lanczos3`、`Gaussian`。

```csharp
image.Resize(256, 256, ResizeFilter.Lanczos3);
```

### [`PixelType`](pixel-type.md)

像素格式枚举，用于判断图片的通道排列方式。

```csharp
if (image.PixelType == PixelType.RGBA) { ... }
```

### [`SampleFormat`](sample-format.md)

音频解码时使用的采样格式枚举，支持 `Float` 和 `Int16`。

### [`IUniassetStream`](iostream.md)

自定义流接口。

```csharp
public interface IUniassetStream
{
    int Read(Span<byte> buffer);
    long Seek(long offset, SeekOrigin origin);
}
```

### [`StreamWrapper`](stream-wrapper.md)

`System.IO.Stream` 到 `IUniassetStream` 的适配器。

## 使用模式

### 同步加载

```csharp
using var asset = new ImageAsset();
asset.Load("path/to/file.png");
```

### 异步加载

```csharp
using var asset = new ImageAsset();
await asset.LoadAsync("path/to/file.png");
```

### 释放资源

`ImageAsset` 和 `AudioAsset` 都实现了 `IDisposable`，使用完成后应及时释放：

```csharp
using var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
```

## 类型列表

| 类型 | 命名空间 | 说明 |
|------|----------|------|
| `ImageAsset` | `Uniasset.Image` | 图片加载、处理与转换 |
| `CropOptions` | `Uniasset.Image` | 批量裁剪参数 |
| `ResizeFilter` | `Uniasset.Image` | 缩放滤波算法 |
| `PixelType` | `Uniasset.Image` | 像素格式 |
| `AudioAsset` | `Uniasset.Audio` | 音频加载、读取与转换 |
| `SampleFormat` | `Uniasset.Audio` | 音频采样格式 |
| `IUniassetStream` | `Uniasset` | 自定义流接口 |
| `SeekOrigin` | `Uniasset` | 流定位基准 |
| `StreamWrapper` | `Uniasset` | `System.IO.Stream` 适配器 |
