# SampleFormat

`Uniasset.Audio.SampleFormat` 是一个枚举，用于指定音频采样格式。

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

### Float

```csharp
Float = 0
```

32 位浮点采样格式。每个采样点为 `float` 类型（4 字节），取值范围 `[-1.0, 1.0]`。

**适用场景：**

- 需要高精度音频处理
- Unity 内部音频处理的默认格式
- 需要进行音频效果处理

### Int16

```csharp
Int16 = 1
```

16 位整数采样格式。每个采样点为 `short` 类型（2 字节），取值范围 `[-32768, 32767]`。

**适用场景：**

- 内存占用更低
- 与某些音频系统兼容性更好
- 不需要浮点精度的场景

## 使用示例

```csharp
using Uniasset;
using Uniasset.Audio;

// 使用 Float 格式（默认）
var audio1 = new AudioAsset();
audio1.Load("music.mp3", SampleFormat.Float);
float[] floatBuffer = new float[1024];
audio1.Read<float>(floatBuffer, 1024);

// 使用 Int16 格式
var audio2 = new AudioAsset();
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
| 直接播放原始数据 | `Int16` |
