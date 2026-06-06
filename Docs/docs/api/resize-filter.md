# ResizeFilter

`Uniasset.Image.ResizeFilter` 枚举指定缩放图片时使用的滤波算法。

## 定义

```csharp
namespace Uniasset.Image
{
    public enum ResizeFilter : uint
    {
        Nearest = 0,
        Box = 1,
        Lanczos3 = 2,
        Gaussian = 3,
    }
}
```

## 成员

| 成员 | 值 | 说明 |
|------|-----|------|
| `Nearest` | `0` | 最近邻插值，速度最快，适合像素风格 |
| `Box` | `1` | 盒式滤波，适合缩小 |
| `Lanczos3` | `2` | Lanczos 3-lobe 插值，质量最高 |
| `Gaussian` | `3` | 高斯滤波，平滑效果 |

## 使用

```csharp
image.Resize(256, 256, ResizeFilter.Lanczos3);
```

不指定 `ResizeFilter` 时默认使用 `Nearest`。
