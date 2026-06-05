# 图片加载指南

本指南介绍如何使用 Uniasset 加载和处理图片。

## 支持格式

Uniasset 支持以下图片格式：

| 格式 | 扩展名 | 说明 |
|------|--------|------|
| JPEG | `.jpg`, `.jpeg` | 有损压缩，适合照片 |
| WebP | `.webp` | 同时支持有损和无损压缩 |
| PNG | `.png` | 无损压缩，支持透明通道 |
| BMP | `.bmp` | 未压缩或轻度压缩，兼容性好 |
| TGA | `.tga` | 常用于游戏资源，支持透明通道 |
| PSD | `.psd` | Photoshop 源文件格式 |

## 基本加载

### 从文件加载

```csharp
using Uniasset.Image;

var image = new ImageAsset();
image.Load("Assets/Images/photo.png");

Debug.Log($"图片尺寸: {image.Width} x {image.Height}");
Debug.Log($"通道数: {image.ChannelCount}");
```

### 从字节数组加载

```csharp
byte[] data = File.ReadAllBytes("photo.png");
image.Load(data);
```

### 从网络加载

```csharp
using var http = new HttpClient();
byte[] data = await http.GetByteArrayAsync("https://example.com/photo.jpg");
await image.LoadAsync(data);
```

## 解码时缩放

在加载图片时可以指定期望的解码尺寸，减少内存占用：

```csharp
// 解码时直接缩放到 256x256
image.Load("large_photo.png", 256, 256);

// 或者只指定一个维度，另一个维度设为 0 表示按比例缩放
image.Load("photo.png", 128, 0);  // 宽度 128，高度按比例
```

## 裁剪

### 单次裁剪

```csharp
image.Crop(x: 100, y: 100, width: 200, height: 200);
```

### 批量裁剪

适用于从精灵图集（Sprite Sheet）中提取多个精灵：

```csharp
var crops = new CropOptions[]
{
    new CropOptions(0, 0, 32, 32),     // 第 1 帧
    new CropOptions(32, 0, 32, 32),    // 第 2 帧
    new CropOptions(64, 0, 32, 32),    // 第 3 帧
    new CropOptions(96, 0, 32, 32),    // 第 4 帧
};

ImageAsset[] frames = image.CropMultiple(crops);
```

## 缩放

```csharp
image.Resize(256, 256);
```

!!! note "缩放算法"
    当前版本使用最近邻插值算法。更多缩放算法将在后续版本中提供。

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
    bool mipmap = false,           // 是否生成 mipmap
    bool linear = true,            // 线性色彩空间
    bool noLongerReadable = true   // 创建后设为不可读（节省内存）
);
```

| 参数 | 说明 |
|------|------|
| `mipmap` | 生成 mipmap 会增加内存占用，但可以提高渲染性能 |
| `linear` | `true` 用于线性色彩空间，`false` 用于 sRGB |
| `noLongerReadable` | 设为 `true` 后无法再读取像素数据，但节省内存 |

## 克隆

```csharp
ImageAsset copy = image.Clone();
```

克隆后两个实例互不影响，可以独立修改。

## 异步操作

所有耗时操作都提供了异步版本：

```csharp
var image = new ImageAsset();

await image.LoadAsync("photo.png");
await image.CropAsync(0, 0, 100, 100);
await image.ResizeAsync(50, 50);
Texture2D texture = await image.ToTexture2DAsync();
```

## 资源管理

### 使用 using 语句

```csharp
using (var image = new ImageAsset())
{
    image.Load("photo.png");
    Texture2D texture = image.ToTexture2D();
    // texture 在 using 块结束后仍然有效
}
```

### 手动释放

```csharp
var image = new ImageAsset();
try
{
    image.Load("photo.png");
    // 使用图片...
}
finally
{
    image.Dispose();
}
```

### 重新加载

```csharp
var image = new ImageAsset();
image.Load("photo1.png");
// 使用...

image.Unload();  // 卸载当前图片
image.Load("photo2.png");  // 加载新图片
```

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

### 加载头像并裁剪为正方形

```csharp
public async Task<Texture2D> LoadAvatar(string path, int size)
{
    using var image = new ImageAsset();
    await image.LoadAsync(path);
    
    // 裁剪为正方形
    int minDim = Math.Min(image.Width, image.Height);
    int x = (image.Width - minDim) / 2;
    int y = (image.Height - minDim) / 2;
    image.Crop(x, y, minDim, minDim);
    
    // 缩放到目标尺寸
    image.Resize(size, size);
    
    return await image.ToTexture2DAsync();
}
```

### 批量加载精灵

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
    Texture2D[] textures = new Texture2D[sprites.Length];
    
    for (int i = 0; i < sprites.Length; i++)
    {
        textures[i] = sprites[i].ToTexture2D();
        sprites[i].Dispose();
    }
    
    return textures;
}
```
