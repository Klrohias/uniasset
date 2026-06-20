# 音频加载指南

本指南介绍如何使用 `Uniasset.Audio.AudioAsset` 加载音频、读取 PCM 数据并转换为 Unity `AudioClip`。

## 支持格式

Uniasset 当前支持以下音频格式：

| 格式 | 扩展名 | 说明 |
|------|--------|------|
| MP3 | `.mp3` | 常见有损压缩格式 |
| FLAC | `.flac` | 无损压缩格式 |
| WAV | `.wav` | 常见未压缩容器 |
| PCM | `.pcm` | 原始 PCM 数据 |
| Vorbis | — | Vorbis 编码音频 |
| OGG | `.ogg` | 常见 Vorbis 容器 |

## 基本加载

### 从文件加载

```csharp
using Uniasset.Audio;

using var audio = new AudioAsset();
audio.Load("Assets/Audio/bgm.mp3");

Debug.Log($"采样率: {audio.SampleRate} Hz");
Debug.Log($"声道数: {audio.ChannelCount}");
Debug.Log($"总帧数: {audio.FrameCount}");
```

### 从字节数组加载

```csharp
using var audio = new AudioAsset();

byte[] data = File.ReadAllBytes("sound.wav");
audio.Load(data);
```

### 从自定义流加载

```csharp
using Uniasset;
using Uniasset.Audio;

using var audio = new AudioAsset();
using var fileStream = File.OpenRead("music.flac");
var stream = new StreamWrapper(fileStream);

audio.LoadIO(stream);
```

!!! note
    当前 `AudioAsset` 只提供 `LoadIO(IUniassetStream)`，没有 `LoadIO(Stream)` 便捷重载。

## 采样格式

加载音频时可以指定输出采样格式：

```csharp
audio.Load("music.mp3", SampleFormat.Float);
audio.Load("music.mp3", SampleFormat.Int16);
```

一般建议：

- 需要直接交给 Unity 或做浮点处理时使用 `SampleFormat.Float`
- 想降低内存占用或直接处理 16 位 PCM 时使用 `SampleFormat.Int16`

详见 [SampleFormat API 参考](../api/sample-format.md)。

## 转换为 AudioClip

### 流式播放

```csharp
AudioClip clip = audio.ToAudioClip("BGM", stream: true);
audioSource.clip = clip;
audioSource.Play();
```

适合：

- 背景音乐
- 长时音频
- 希望降低一次性内存占用的场景

### 全量加载

```csharp
AudioClip clip = audio.ToAudioClip("Explosion", stream: false);
```

适合：

- 短音效
- 需要频繁重复播放的片段

## 手动读取 PCM 数据

### 基本读取

```csharp
using var audio = new AudioAsset();
audio.Load("music.flac", SampleFormat.Float);

float[] buffer = new float[1024 * audio.ChannelCount];
int framesRead = audio.Read<float>(buffer, 1024);
```

### 逐块读取整个文件

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3", SampleFormat.Float);

int chunkSize = 4096;
float[] buffer = new float[chunkSize * audio.ChannelCount];
long totalFrames = 0;

while (true)
{
    int framesRead = audio.Read<float>(buffer, chunkSize);
    if (framesRead == 0) break;

    totalFrames += framesRead;
}

Debug.Log($"总共读取 {totalFrames} 帧");
```

### 使用 Int16 格式

```csharp
using var audio = new AudioAsset();
audio.Load("sound.wav", SampleFormat.Int16);

short[] buffer = new short[1024 * audio.ChannelCount];
int framesRead = audio.Read<short>(buffer, 1024);
```

!!! warning
    `Read<T>(...)` 中的 `T` 必须与加载时使用的 `SampleFormat` 对应：
    `SampleFormat.Float -> float`，`SampleFormat.Int16 -> short`。

## 定位与跳转

### 获取当前位置

```csharp
long currentFrame = audio.Tell();
Debug.Log($"当前帧: {currentFrame} / {audio.FrameCount}");
```

### 跳转到指定帧

```csharp
audio.Seek(1000);
audio.Seek(0);
```

### 秒数与帧数换算

```csharp
float seconds = (float)audio.Tell() / audio.SampleRate;

long frame = (long)(10.5f * audio.SampleRate);
audio.Seek(frame);
```

## 生命周期管理

### 使用 using

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
AudioClip clip = audio.ToAudioClip();
```

### 卸载已加载数据

```csharp
audio.Unload();
```

`Unload()` 会清空当前音频内容，但对象本身仍可继续复用。

## 常见用例

### 背景音乐播放器

```csharp
public class BGMPlayer : MonoBehaviour
{
    [SerializeField] private AudioSource _audioSource;

    private AudioAsset _audioAsset;

    public void PlayBGM(string path)
    {
        StopBGM();

        _audioAsset = new AudioAsset();
        _audioAsset.Load(path);

        _audioSource.clip = _audioAsset.ToAudioClip("BGM", stream: true);
        _audioSource.loop = true;
        _audioSource.Play();
    }

    public void StopBGM()
    {
        _audioSource.Stop();
        _audioSource.clip = null;
        _audioAsset?.Dispose();
        _audioAsset = null;
    }

    private void OnDestroy()
    {
        StopBGM();
    }
}
```

### 音效缓存

```csharp
public class SFXManager : MonoBehaviour
{
    private readonly Dictionary<string, AudioClip> _cache = new();

    public AudioClip LoadSFX(string path)
    {
        if (_cache.TryGetValue(path, out var cached))
            return cached;

        using var audio = new AudioAsset();
        audio.Load(path);

        var clip = audio.ToAudioClip(Path.GetFileName(path), stream: false);
        _cache[path] = clip;
        return clip;
    }
}
```

### 波形预览

```csharp
public float[] ReadWaveform(string path, int frameCount)
{
    using var audio = new AudioAsset();
    audio.Load(path, SampleFormat.Float);

    float[] samples = new float[frameCount * audio.ChannelCount];
    int framesRead = audio.Read<float>(samples, frameCount);

    if (framesRead < frameCount)
    {
        Array.Resize(ref samples, framesRead * audio.ChannelCount);
    }

    return samples;
}
```

### 截取指定时间段

```csharp
public float[] ExtractSegment(string path, float startSec, float durationSec)
{
    using var audio = new AudioAsset();
    audio.Load(path, SampleFormat.Float);

    long startFrame = (long)(startSec * audio.SampleRate);
    int frameCount = (int)(durationSec * audio.SampleRate);

    audio.Seek(startFrame);

    float[] buffer = new float[frameCount * audio.ChannelCount];
    int framesRead = audio.Read<float>(buffer, frameCount);

    if (framesRead < frameCount)
    {
        Array.Resize(ref buffer, framesRead * audio.ChannelCount);
    }

    return buffer;
}
```
