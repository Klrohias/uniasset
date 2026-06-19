# PixelType

`Uniasset.Image.PixelType` describes the pixel layout of an image.

## Definition

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

## Members

| Member | Value | Channel count |
|------|-----|--------|
| `Unknown` | `0` | 0 |
| `RGBA` | `1` | 4 |
| `ARGB` | `2` | 4 |
| `RGB` | `3` | 3 |
| `Grey` | `4` | 1 |

## Example

```csharp
using var image = new ImageAsset();
image.Load("photo.png");

if (image.PixelType == PixelType.RGBA)
{
    Debug.Log("The image contains an alpha channel");
}
```

You can also read the channel count directly from `ImageAsset.ChannelCount`.
