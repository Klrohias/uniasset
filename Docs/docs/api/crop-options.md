# CropOptions

`Uniasset.Image.CropOptions` 是一个结构体，用于描述裁剪区域。

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

### X

```csharp
public uint X;
```

裁剪区域左边界坐标。

### Y

```csharp
public uint Y;
```

裁剪区域上边界坐标。

### Width

```csharp
public uint Width;
```

裁剪区域宽度。

### Height

```csharp
public uint Height;
```

裁剪区域高度。

## 构造函数

```csharp
public CropOptions(int x, int y, int width, int height)
```

使用 `checked` 算术创建实例，确保值不为负数。

**参数：**

| 参数 | 类型 | 说明 |
|------|------|------|
| `x` | `int` | 左边界坐标 |
| `y` | `int` | 上边界坐标 |
| `width` | `int` | 裁剪宽度 |
| `height` | `int` | 裁剪高度 |

**异常：** 如果任一参数为负数，将抛出 `OverflowException`。

## 使用示例

```csharp
using Uniasset.Image;

// 创建裁剪选项
var crop = new CropOptions(10, 20, 100, 100);

// 批量裁剪
var crops = new CropOptions[]
{
    new CropOptions(0, 0, 32, 32),      // 左上角
    new CropOptions(32, 0, 32, 32),     // 右移一格
    new CropOptions(0, 32, 32, 32),     // 下移一格
    new CropOptions(32, 32, 32, 32),    // 右下角
};

ImageAsset[] results = image.CropMultiple(crops);
```
