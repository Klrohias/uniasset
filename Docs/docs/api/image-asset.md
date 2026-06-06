# ImageAsset

`Uniasset.Image.ImageAsset` 是图片资源的高层封装，支持加载、裁剪、缩放、克隆以及转换为 Unity `Texture2D`。

## 类定义

```csharp
namespace Uniasset.Image
{
    public sealed class ImageAsset : IDisposable, ICloneable { }
}
```

## 构造函数

```csharp
public ImageAsset()
```

创建一个空的 `ImageAsset` 实例。

## 只读属性

| 属性 | 类型 | 说明 |
|------|------|------|
| `Width` | `int` | 当前图片宽度 |
| `Height` | `int` | 当前图片高度 |
| `ChannelCount` | `int` | 当前图片通道数 |
| `PixelType` | `PixelType` | 当前图片像素类型 |

`ChannelCount` 常见值：

- `3`: RGB
- `4`: RGBA

## 加载

### Load

```csharp
public void Load(string path, int expectedWidth = 0, int expectedHeight = 0)
```

从文件路径加载图片。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `path` | `string` | — | 图片文件路径 |
| `expectedWidth` | `int` | `0` | 期望的解码宽度，`0` 表示使用原始宽度 |
| `expectedHeight` | `int` | `0` | 期望的解码高度，`0` 表示使用原始高度 |

```csharp
var image = new ImageAsset();
image.Load("photo.png");
image.Load("photo.png", 256, 256);
```

`expectedWidth` 和 `expectedHeight` 小于 `0` 时会抛出 `ArgumentOutOfRangeException`。

### Load(Span<byte>)

```csharp
public void Load(Span<byte> data, int expectedWidth = 0, int expectedHeight = 0)
```

从内存中的编码图片数据加载。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `data` | `Span<byte>` | — | 图片数据 |
| `expectedWidth` | `int` | `0` | 期望的解码宽度 |
| `expectedHeight` | `int` | `0` | 期望的解码高度 |

```csharp
byte[] imageData = File.ReadAllBytes("photo.png");
image.Load(imageData);
```

### LoadAsync

```csharp
public Task LoadAsync(string path, int expectedWidth = 0, int expectedHeight = 0)
```

异步从文件路径加载图片。

```csharp
await image.LoadAsync("photo.png");
```

### LoadAsync(byte[])

```csharp
public Task LoadAsync(byte[] data, int expectedWidth = 0, int expectedHeight = 0)
```

异步从 `byte[]` 加载图片。

```csharp
byte[] data = File.ReadAllBytes("photo.png");
await image.LoadAsync(data);
```

### LoadIO

```csharp
public void LoadIO(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
```

从 `IUniassetStream` 加载图片。

### LoadIO(Stream)

```csharp
public void LoadIO(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
```

从 `System.IO.Stream` 加载图片。内部会自动包装为 [`StreamWrapper`](stream-wrapper.md)。

### LoadIOAsync

```csharp
public Task LoadIOAsync(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
public Task LoadIOAsync(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
```

异步从流加载图片。

## 资源生命周期

### Unload

```csharp
public void Unload()
```

卸载当前已加载的图片数据，但保留 `ImageAsset` 实例，可继续重新加载。

### Dispose

```csharp
public void Dispose()
```

释放底层资源。释放后不应继续使用该实例。

## 图像处理

### Crop

```csharp
public void Crop(int x, int y, int width, int height)
```

裁剪图片（原地修改）。

| 参数 | 类型 | 说明 |
|------|------|------|
| `x` | `int` | 左边界 |
| `y` | `int` | 上边界 |
| `width` | `int` | 裁剪宽度 |
| `height` | `int` | 裁剪高度 |

```csharp
image.Crop(10, 10, 100, 100);
```

### CropAsync

```csharp
public Task CropAsync(int x, int y, int width, int height)
```

异步裁剪图片。

### CropMultiple

```csharp
public ImageAsset[] CropMultiple(CropOptions[] optionsArray)
```

批量裁剪，返回多个新的 `ImageAsset` 实例。

| 参数 | 类型 | 说明 |
|------|------|------|
| `optionsArray` | `CropOptions[]` | 裁剪区域数组 |

返回值与输入数组顺序一一对应。

```csharp
var crops = new[]
{
    new CropOptions(0, 0, 50, 50),
    new CropOptions(50, 0, 50, 50),
};
ImageAsset[] results = image.CropMultiple(crops);
```

### CropMultipleAsync

```csharp
public Task<ImageAsset[]> CropMultipleAsync(CropOptions[] optionsArray)
```

异步批量裁剪。

### Resize

```csharp
public void Resize(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
```

缩放图片（原地修改）。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `width` | `int` | — | 目标宽度 |
| `height` | `int` | — | 目标高度 |
| `filter` | `ResizeFilter` | `Nearest` | 缩放滤波算法 |

```csharp
image.Resize(256, 256);
image.Resize(256, 256, ResizeFilter.Lanczos3);
```

### ResizeAsync

```csharp
public Task ResizeAsync(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
```

异步缩放图片。

## 转换

### ToTexture2D

```csharp
public Texture2D ToTexture2D(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
)
```

将当前图片转换为 Unity `Texture2D`。

仅支持 `ChannelCount` 为 `3` 或 `4` 的图片；否则会抛出 `NotSupportedException`。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `mipmap` | `bool` | `false` | 是否生成 mipmap |
| `linear` | `bool` | `true` | 是否使用线性色彩空间 |
| `noLongerReadable` | `bool` | `true` | 创建后是否设为不可读 |

```csharp
Texture2D texture = image.ToTexture2D();
```

### ToTexture2DAsync

```csharp
public Task<Texture2D> ToTexture2DAsync(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
)
```

异步拷贝像素数据并返回 `Texture2D`。

!!! note
    该方法内部仍会创建 Unity `Texture2D` 并调用 `Apply`，实际使用时仍应保证调用环境符合 Unity 对象访问约束。

## 复制

### Clone

```csharp
public ImageAsset Clone()
```

深拷贝当前图片资源。

```csharp
ImageAsset copy = image.Clone();
```

`ImageAsset` 还显式实现了 `ICloneable.Clone()`，返回值等价于 `Clone()`。