# 图片加载指南

本指南介绍如何使用 `Uniasset.Image.ImageAsset` 加载、处理并转换图片资源。

## 支持格式

Uniasset 当前支持以下图片格式：

| 格式 | 扩展名 | 说明 |
|------|--------|------|
| JPEG | `.jpg`, `.jpeg` | 有损压缩，适合照片 |
| WebP | `.webp` | 支持有损和无损压缩 |
| PNG | `.png` | 无损压缩，支持透明通道 |
| BMP | `.bmp` | 兼容性好 |
| TGA | `.tga` | 常用于游戏资源 |
| PSD | `.psd` | Photoshop 源文件格式 |

## 基本加载

### 从文件加载

```csharp
using Uniasset.Image;

using var image = new ImageAsset();
image.Load("Assets/Images/photo.png");

Debug.Log($"图片尺寸: {image.Width} x {image.Height}");
Debug.Log($"通道数: {image.ChannelCount}");
```

### 从字节数组加载

```csharp
using var image = new ImageAsset();

byte[] data = File.ReadAllBytes("photo.png");
image.Load(data);
```

### 从流加载

```csharp
using Uniasset.Image;

using var image = new ImageAsset();
using var stream = File.OpenRead("photo.png");

image.LoadIO(stream);
```

### 异步加载

```csharp
using var image = new ImageAsset();

await image.LoadAsync("photo.png");
```

!!! note
    当前异步加载仅提供 `LoadAsync(string)` 和 `LoadAsync(byte[])`。`LoadIO(...)` 没有异步重载。

## 解码时指定目标尺寸

加载图片时可以传入 `expectedWidth` 和 `expectedHeight`，在解码阶段直接输出目标尺寸，减少后续内存和处理开销：

```csharp
image.Load("large_photo.png", expectedWidth: 256, expectedHeight: 256);
```

如果不需要指定尺寸，传 `0` 即可：

```csharp
image.Load("photo.png", expectedWidth: 0, expectedHeight: 0);
```

!!! warning
    `expectedWidth` 和 `expectedHeight` 不能小于 `0`，否则会抛出 `ArgumentOutOfRangeException`。

## 图片处理

### 裁剪

```csharp
image.Crop(x: 100, y: 100, width: 200, height: 200);
```

### 异步裁剪

```csharp
await image.CropAsync(100, 100, 200, 200);
```

### 批量裁剪

适合从图集里批量提取子图：

```csharp
var crops = new CropOptions[]
{
    new CropOptions(0, 0, 32, 32),
    new CropOptions(32, 0, 32, 32),
    new CropOptions(64, 0, 32, 32),
    new CropOptions(96, 0, 32, 32),
};

ImageAsset[] frames = image.CropMultiple(crops);
```

### 异步批量裁剪

```csharp
ImageAsset[] frames = await image.CropMultipleAsync(crops);
```

### 缩放

```csharp
image.Resize(256, 256);
```

### 异步缩放

```csharp
await image.ResizeAsync(256, 256);
```

## 转换为 Texture2D

### 同步转换

```csharp
Texture2D texture = image.ToTexture2D();
```

### 异步转换

```csharp
Texture2D texture = await image.ToTexture2DAsync();
```

### 参数说明

```csharp
Texture2D ToTexture2D(
    bool mipmap = false,
    bool linear = true,
    bool noLongerReadable = true
);
```

| 参数 | 说明 |
|------|------|
| `mipmap` | 是否生成 mipmap |
| `linear` | 是否按线性色彩空间创建纹理 |
| `noLongerReadable` | 创建后是否让纹理变为不可读 |

!!! note
    `ToTexture2D()` 和 `ToTexture2DAsync()` 仅支持通道数为 `3` 或 `4` 的图片数据。

## 克隆

```csharp
ImageAsset copy = image.Clone();
```

克隆后得到新的 `ImageAsset` 实例，后续处理互不影响。

## 生命周期管理

### 使用 using

```csharp
using var image = new ImageAsset();
image.Load("photo.png");
Texture2D texture = image.ToTexture2D();
```

### 卸载已加载数据

```csharp
image.Unload();
```

`Unload()` 会释放当前图片内容，但保留 `ImageAsset` 对象本身，之后仍可继续 `Load(...)`。

## 常见用例

### 加载 UI 图片

```csharp
public class ImageLoader : MonoBehaviour
{
    [SerializeField] private RawImage _rawImage;

    private ImageAsset _imageAsset;

    private async void Start()
    {
        _imageAsset = new ImageAsset();
        await _imageAsset.LoadAsync("Assets/UI/background.png");
        _rawImage.texture = await _imageAsset.ToTexture2DAsync();
    }

    private void OnDestroy()
    {
        _imageAsset?.Dispose();
    }
}
```

### 读取头像并裁剪为正方形

```csharp
public async Task<Texture2D> LoadAvatar(string path, int size)
{
    using var image = new ImageAsset();
    await image.LoadAsync(path);

    int minDim = Math.Min(image.Width, image.Height);
    int x = (image.Width - minDim) / 2;
    int y = (image.Height - minDim) / 2;

    image.Crop(x, y, minDim, minDim);
    image.Resize(size, size);

    return await image.ToTexture2DAsync();
}
```

### 切分图集

```csharp
public async Task<Texture2D[]> LoadSpriteSheet(string path, int spriteWidth, int spriteHeight)
{
    using var sheet = new ImageAsset();
    await sheet.LoadAsync(path);

    int cols = sheet.Width / spriteWidth;
    int rows = sheet.Height / spriteHeight;

    var crops = new CropOptions[cols * rows];
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            crops[y * cols + x] = new CropOptions(
                x * spriteWidth,
                y * spriteHeight,
                spriteWidth,
                spriteHeight
            );
        }
    }

    ImageAsset[] sprites = sheet.CropMultiple(crops);
    try
    {
        var textures = new Texture2D[sprites.Length];
        for (int i = 0; i < sprites.Length; i++)
        {
            textures[i] = sprites[i].ToTexture2D();
        }

        return textures;
    }
    finally
    {
        foreach (var sprite in sprites)
        {
            sprite.Dispose();
        }
    }
}
```
