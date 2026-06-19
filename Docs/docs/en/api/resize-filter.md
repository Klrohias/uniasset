# ResizeFilter

`Uniasset.Image.ResizeFilter` selects the filtering algorithm used when resizing images.

## Definition

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

## Members

| Member | Value | Description |
|------|-----|------|
| `Nearest` | `0` | Nearest-neighbor interpolation; fastest and good for pixel art |
| `Box` | `1` | Box filter; useful for downscaling |
| `Lanczos3` | `2` | Lanczos 3-lobe interpolation; highest quality |
| `Gaussian` | `3` | Gaussian filter with smoother output |

## Example

```csharp
image.Resize(256, 256, ResizeFilter.Lanczos3);
```

If omitted, `ResizeFilter.Nearest` is used by default.
