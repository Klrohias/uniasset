# SampleFormat

`Uniasset.Audio.SampleFormat` 用于指定音频解码后的样本格式。

## 枚举定义

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

## 成员

| 成员 | 值 | 说明 |
|------|----|------|
| `Float` | `0` | 32 位浮点格式，对应 `Read<float>(...)` |
| `Int16` | `1` | 16 位整数格式，对应 `Read<short>(...)` |

## 使用示例

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

## 选择建议

| 场景 | 推荐格式 |
|------|----------|
| 通用场景 | `Float` |
| 内存受限 | `Int16` |
| 需要音频处理 | `Float` |
| 直接处理整数 PCM | `Int16` |
