# CropOptions

`Uniasset.Image.CropOptions` describes a crop rectangle and is primarily used with `ImageAsset.CropMultiple(...)`.

## Definition

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

## Fields

| Field | Type | Description |
|------|------|------|
| `X` | `uint` | Top-left X coordinate |
| `Y` | `uint` | Top-left Y coordinate |
| `Width` | `uint` | Crop width |
| `Height` | `uint` | Crop height |

## Constructor

```csharp
public CropOptions(int x, int y, int width, int height)
```

Creates an instance with `checked` arithmetic to ensure values are not negative.

| Parameter | Type | Description |
|------|------|------|
| `x` | `int` | Left boundary |
| `y` | `int` | Top boundary |
| `width` | `int` | Crop width |
| `height` | `int` | Crop height |

If any parameter is negative, the constructor throws `OverflowException`.

## Example

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
