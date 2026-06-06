# CropOptions

`Uniasset.Image.CropOptions` 用于描述一个裁剪矩形，主要配合 `ImageAsset.CropMultiple(...)` 使用。

## 结构体定义

```csharp
namespace Uniasset.Image
{
    public struct CropOptions
    {
        public uint X;
        public uint Y;
        public uint Width;
        public uint Height;

        public CropOptions(int x, int y, int width, int height);
    }
}
```

## 字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `X` | `uint` | 左上角 X 坐标 |
| `Y` | `uint` | 左上角 Y 坐标 |
| `Width` | `uint` | 裁剪宽度 |
| `Height` | `uint` | 裁剪高度 |

## 构造函数

```csharp
public CropOptions(int x, int y, int width, int height)
```

使用 `checked` 算术创建实例，确保值不为负数。

| 参数 | 类型 | 说明 |
|------|------|------|
| `x` | `int` | 左边界坐标 |
| `y` | `int` | 上边界坐标 |
| `width` | `int` | 裁剪宽度 |
| `height` | `int` | 裁剪高度 |

如果任一参数为负数，将抛出 `OverflowException`。

## 使用示例

```csharp
using Uniasset.Image;

var crops = new CropOptions[]
{
    new CropOptions(0, 0, 32, 32),
    new CropOptions(32, 0, 32, 32),
    new CropOptions(64, 0, 32, 32),
};

ImageAsset[] results = image.CropMultiple(crops);
```
