# ImageAsset

`Uniasset.Image.ImageAsset` 是 Uniasset 图片模块的核心类，提供图片加载、处理和转换功能。

## 类定义

```csharp
namespace Uniasset.Image
{
    public sealed class ImageAsset : IDisposable, ICloneable { }
}
```

## 属性

### Width

```csharp
public int Width { get; }
```

图片宽度（像素）。

### Height

```csharp
public int Height { get; }
```

图片高度（像素）。

### ChannelCount

```csharp
public int ChannelCount { get; }
```

图片通道数。

| 值 | 含义 |
|:--:|------|
| 1  | 灰度 (Grey) |
| 3  | RGB |
| 4  | RGBA / ARGB |

### UnsafeHandle

```csharp
public UnsafeImageAsset UnsafeHandle { get; }
```

获取底层的不安全句柄。仅在需要直接调用 FFI 时使用。

## 方法

### Load

```csharp
public void Load(string path, int expectedWidth = 0, int expectedHeight = 0)
```

从文件路径加载图片。

**参数：**

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `path` | `string` | — | 图片文件路径 |
| `expectedWidth` | `int` | `0` | 期望的解码宽度，`0` 表示使用原始宽度 |
| `expectedHeight` | `int` | `0` | 期望的解码高度，`0` 表示使用原始高度 |

```csharp
var image = new ImageAsset();
image.Load("photo.png");

// 解码时缩放到指定尺寸
image.Load("photo.png", 256, 256);
```

---

### Load (byte[])

```csharp
public void Load(Span<byte> data, int expectedWidth = 0, int expectedHeight = 0)
```

从字节数组加载图片。

**参数：**

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `data` | `Span<byte>` | — | 图片数据 |
| `expectedWidth` | `int` | `0` | 期望的解码宽度 |
| `expectedHeight` | `int` | `0` | 期望的解码高度 |

```csharp
byte[] imageData = File.ReadAllBytes("photo.png");
image.Load(imageData);
```

---

### LoadAsync

```csharp
public Task LoadAsync(string path, int expectedWidth = 0, int expectedHeight = 0)
```

异步从文件路径加载图片。

```csharp
await image.LoadAsync("photo.png");
```

---

### LoadAsync (byte[])

```csharp
public Task LoadAsync(byte[] data, int expectedWidth = 0, int expectedHeight = 0)
```

异步从字节数组加载图片。

```csharp
byte[] data = await File.ReadAllBytesAsync("photo.png");
await image.LoadAsync(data);
```

---

### LoadIO

```csharp
public void LoadIO(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
```

从自定义流加载图片。详见 [自定义流](custom-streams.md)。

---

### LoadIO (Stream)

```csharp
public void LoadIO(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
```

从 `System.IO.Stream` 加载图片。内部使用 `StreamWrapper` 适配。

---

### Crop

```csharp
public void Crop(int x, int y, int width, int height)
```

裁剪图片（原地修改）。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `x` | `int` | 左边界 |
| `y` | `int` | 上边界 |
| `width` | `int` | 裁剪宽度 |
| `height` | `int` | 裁剪高度 |

```csharp
image.Crop(10, 10, 100, 100);
```

---

### CropAsync

```csharp
public Task CropAsync(int x, int y, int width, int height)
```

异步裁剪图片。

---

### CropMultiple

```csharp
public ImageAsset[] CropMultiple(CropOptions[] optionsArray)
```

批量裁剪，返回多个新的 `ImageAsset` 实例。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `optionsArray` | `CropOptions[]` | 裁剪区域数组 |

**返回值：** 裁剪后的 `ImageAsset` 数组，与输入数组一一对应。

```csharp
var crops = new[]
{
    new CropOptions(0, 0, 50, 50),
    new CropOptions(50, 0, 50, 50),
};
ImageAsset[] results = image.CropMultiple(crops);
```

---

### CropMultipleAsync

```csharp
public Task<ImageAsset[]> CropMultipleAsync(CropOptions[] optionsArray)
```

异步批量裁剪。

---

### Resize

```csharp
public void Resize(int width, int height)
```

缩放图片（原地修改）。使用最近邻插值算法。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `width` | `int` | 目标宽度 |
| `height` | `int` | 目标高度 |

```csharp
image.Resize(256, 256);
```

---

### ResizeAsync

```csharp
public Task ResizeAsync(int width, int height)
```

异步缩放图片。

---

### ToTexture2D

```csharp
public Texture2D ToTexture2D(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
)
```

将图片转换为 Unity `Texture2D`。**必须在主线程上调用。**

**参数：**

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `mipmap` | `bool` | `false` | 是否生成 mipmap |
| `linear` | `bool` | `true` | 是否使用线性色彩空间 |
| `noLongerReadable` | `bool` | `true` | 创建后是否设为不可读（节省内存） |

**返回值：** 创建的 `Texture2D` 实例。

```csharp
Texture2D texture = image.ToTexture2D();
```

---

### ToTexture2DAsync

```csharp
public Task<Texture2D> ToTexture2DAsync(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
)
```

异步转换为 `Texture2D`。

---

### Clone

```csharp
public ImageAsset Clone()
```

深拷贝图片资源。

```csharp
ImageAsset copy = image.Clone();
```

---

### Unload

```csharp
public void Unload()
```

卸载已加载的图片数据。卸载后可以重新调用 `Load` 方法加载新图片。

---

### Dispose

```csharp
public void Dispose()
```

释放所有资源。释放后不可再使用此实例。

## 示例

### 完整的图片处理流程

```csharp
using Uniasset;
using Uniasset.Image;

// 加载图片
using var image = new ImageAsset();
await image.LoadAsync("photo.png");

// 裁剪
await image.CropAsync(100, 100, 500, 500);

// 缩放
await image.ResizeAsync(256, 256);

// 转换为 Texture2D
Texture2D texture = await image.ToTexture2DAsync();
```

### 批量裁剪

```csharp
using var source = new ImageAsset();
source.Load("spritesheet.png");

var crops = new[]
{
    new CropOptions(0, 0, 32, 32),
    new CropOptions(32, 0, 32, 32),
    new CropOptions(64, 0, 32, 32),
};

ImageAsset[] sprites = source.CropMultiple(crops);

// 转换为 Texture2D 数组
Texture2D[] textures = new Texture2D[sprites.Length];
for (int i = 0; i < sprites.Length; i++)
{
    textures[i] = sprites[i].ToTexture2D();
    sprites[i].Dispose();
}
```
