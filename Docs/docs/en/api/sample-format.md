# SampleFormat

`Uniasset.Audio.SampleFormat` specifies the decoded audio sample format.

## Definition

```csharp
namespace Uniasset.Audio
{
    public enum SampleFormat : byte
    {
        Float = 0,
        Int16 = 1
    }
}
```

## Members

| Member | Value | Description |
|------|----|------|
| `Float` | `0` | 32-bit floating-point samples, matching `Read<float>(...)` |
| `Int16` | `1` | 16-bit integer samples, matching `Read<short>(...)` |

## Example

```csharp
using Uniasset.Audio;

using var audio1 = new AudioAsset();
audio1.Load("music.mp3", SampleFormat.Float);
float[] floatBuffer = new float[1024];
audio1.Read<float>(floatBuffer, 1024);

using var audio2 = new AudioAsset();
audio2.Load("music.mp3", SampleFormat.Int16);
short[] shortBuffer = new short[1024];
audio2.Read<short>(shortBuffer, 1024);
```

## Recommendations

| Scenario | Recommended format |
|------|----------|
| General use | `Float` |
| Memory-constrained workloads | `Int16` |
| Audio processing | `Float` |
| Integer PCM workflows | `Int16` |
