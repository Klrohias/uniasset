# API 参考

Uniasset 的 C# SDK 提供了高层级的 API，用于在 Unity 中加载和处理图片与音频资源。

## 命名空间概览

| 命名空间 | 说明 |
|----------|------|
| `Uniasset` | 核心类型，包括流接口和流包装器 |
| `Uniasset.Image` | 图片相关类型 |
| `Uniasset.Audio` | 音频相关类型 |

## 核心类型

### [`ImageAsset`](image-asset.md)

图片资源类，支持从文件、字节数组或自定义流加载图片，提供裁剪、缩放、克隆等操作。

```csharp
var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
image.Dispose();
```

### [`AudioAsset`](audio-asset.md)

音频资源类，支持从文件、字节数组或自定义流加载音频，提供读取 PCM 数据、转换为 AudioClip 等操作。

```csharp
var audio = new AudioAsset();
audio.Load("music.mp3");
AudioClip clip = audio.ToAudioClip();
audio.Dispose();
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

### [`SampleFormat`](sample-format.md)

音频采样格式枚举，支持 `Float` 和 `Int16` 两种格式。

### [`IUniassetStream`](iostream.md)

自定义流接口，允许从任意数据源加载资源。

```csharp
public interface IUniassetStream
{
    int Read(Span<byte> buffer);
    long Seek(long offset, SeekOrigin origin);
}
```

### [`StreamWrapper`](iostream.md#streamwrapper)

`System.IO.Stream` 到 `IUniassetStream` 的适配器。

## 使用模式

### 同步加载

```csharp
var asset = new ImageAsset();
asset.Load("path/to/file.png");
// 使用资源...
asset.Dispose();
```

### 异步加载

```csharp
var asset = new ImageAsset();
await asset.LoadAsync("path/to/file.png");
// 使用资源...
asset.Dispose();
```

### 使用 using 语句

由于 `ImageAsset` 和 `AudioAsset` 都实现了 `IDisposable`，推荐使用 `using` 语句：

```csharp
using (var image = new ImageAsset())
{
    image.Load("photo.png");
    Texture2D texture = image.ToTexture2D();
    // texture 在 using 块结束后仍然有效
}
```

## 异常处理

当原生库发生错误时，会抛出 `NativeException`。建议在调用可能失败的操作时使用 try-catch：

```csharp
try
{
    var image = new ImageAsset();
    image.Load("nonexistent.png");
}
catch (NativeException e)
{
    Debug.LogError($"加载失败: {e.Message}");
}
```
