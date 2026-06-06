# PixelType

`Uniasset.Image.PixelType` 枚举表示图片的像素格式。

## 定义

```csharp
namespace Uniasset.Image
{
    public enum PixelType : uint
    {
        Unknown = 0,
        RGBA = 1,
        ARGB = 2,
        RGB = 3,
        Grey = 4,
    }
}
```

## 成员

| 成员 | 值 | 通道数 |
|------|-----|--------|
| `Unknown` | `0` | 0 |
| `RGBA` | `1` | 4 |
| `ARGB` | `2` | 4 |
| `RGB` | `3` | 3 |
| `Grey` | `4` | 1 |

## 使用

```csharp
using var image = new ImageAsset();
image.Load("photo.png");

if (image.PixelType == PixelType.RGBA)
{
    Debug.Log("图片包含 alpha 通道");
}
```

通过 `ImageAsset.ChannelCount` 属性可直接获取通道数，无需手动映射。
