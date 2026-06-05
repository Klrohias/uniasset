# 音频加载指南

本指南介绍如何使用 Uniasset 加载和处理音频。

## 支持格式

Uniasset 通过 Symphonia 库支持以下音频格式：

| 格式 | 扩展名 | 说明 |
|------|--------|------|
| MP3 | `.mp3` | 有损压缩，最常用的音频格式 |
| FLAC | `.flac` | 无损压缩，高质量 |
| WAV | `.wav` | 无压缩，兼容性好 |
| PCM | `.pcm` | 原始 PCM 数据 |
| Vorbis | — | 开源有损压缩 |
| OGG | `.ogg` | OGG 容器格式 |
| AAC | `.aac`, `.m4a` | 高效有损压缩 |

## 基本加载

### 从文件加载

```csharp
using Uniasset.Audio;

var audio = new AudioAsset();
audio.Load("Assets/Audio/bgm.mp3");

Debug.Log($"采样率: {audio.SampleRate} Hz");
Debug.Log($"声道数: {audio.ChannelCount}");
Debug.Log($"总帧数: {audio.FrameCount}");
```

### 从字节数组加载

```csharp
byte[] data = File.ReadAllBytes("sound.wav");
audio.Load(data);
```

## 采样格式

加载音频时可以指定采样格式：

```csharp
// Float 格式（默认）
audio.Load("music.mp3", SampleFormat.Float);

// Int16 格式（更省内存）
audio.Load("music.mp3", SampleFormat.Int16);
```

详见 [SampleFormat API 参考](../api/sample-format.md)。

## 转换为 AudioClip

### 流式播放（推荐用于长音频）

```csharp
AudioClip clip = audio.ToAudioClip("BGM", stream: true);
audioSource.clip = clip;
audioSource.Play();
```

流式播放在播放时按需读取数据，内存占用低，适合：

- 背景音乐
- 播客
- 长音频

### 全量加载（推荐用于短音效）

```csharp
AudioClip clip = audio.ToAudioClip("Explosion", stream: false);
```

全量加载将所有数据预加载到内存，播放延迟低，适合：

- 游戏音效
- UI 音效
- 短音频片段

## 手动读取 PCM 数据

### 基本读取

```csharp
var audio = new AudioAsset();
audio.Load("music.flac", SampleFormat.Float);

// 读取 1024 帧
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
    // 处理 buffer 中的数据...
    
    // 注意：最后一次读取可能不足 chunkSize 帧
}

Debug.Log($"总共读取 {totalFrames} 帧");
```

### 使用 Int16 格式

```csharp
using var audio = new AudioAsset();
audio.Load("sound.wav", SampleFormat.Int16);

short[] buffer = new short[2048];
int framesRead = audio.Read<short>(buffer, 1024);
```

## 定位与跳转

### 获取当前位置

```csharp
long currentFrame = audio.Tell();
Debug.Log($"当前帧: {currentFrame} / {audio.FrameCount}");
```

### 跳转到指定位置

```csharp
// 跳转到第 1000 帧
audio.Seek(1000);

// 跳转到开头
audio.Seek(0);
```

### 计算时间位置

```csharp
// 帧数转秒数
float seconds = (float)audio.Tell() / audio.SampleRate;

// 秒数转帧数
long frame = (long)(10.5f * audio.SampleRate);
audio.Seek(frame);
```

## 异步操作

```csharp
var audio = new AudioAsset();
await Task.Run(() => audio.Load("large_file.flac"));

// 注意：ToAudioClip 不是异步的，因为它主要是内存操作
AudioClip clip = audio.ToAudioClip();
```

## 资源管理

### 使用 using 语句

```csharp
using (var audio = new AudioAsset())
{
    audio.Load("music.mp3");
    AudioClip clip = audio.ToAudioClip();
    // clip 在 using 块结束后仍然有效
}
```

### 手动释放

```csharp
var audio = new AudioAsset();
try
{
    audio.Load("music.mp3");
    // 使用音频...
}
finally
{
    audio.Dispose();
}
```

### 重新加载

```csharp
var audio = new AudioAsset();
audio.Load("music1.mp3");
// 使用...

audio.Unload();  // 卸载当前音频
audio.Load("music2.mp3");  // 加载新音频
```

## 常见用例

### 背景音乐播放器

```csharp
public class BGMPlayer : MonoBehaviour
{
    [SerializeField] private AudioSource _audioSource;
    
    private AudioAsset _audioAsset;

    public async void PlayBGM(string path)
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

### 音效管理器

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

    public void PlaySFX(string path)
    {
        var clip = LoadSFX(path);
        AudioSource.PlayClipAtPoint(clip, Vector3.zero);
    }

    private void OnDestroy()
    {
        foreach (var clip in _cache.Values)
            Destroy(clip);
        _cache.Clear();
    }
}
```

### 音频波形可视化

```csharp
public class WaveformVisualizer : MonoBehaviour
{
    [SerializeField] private LineRenderer _lineRenderer;
    [SerializeField] private int _sampleCount = 256;

    public void Visualize(string audioPath)
    {
        using var audio = new AudioAsset();
        audio.Load(audioPath, SampleFormat.Float);
        
        float[] samples = new float[_sampleCount * audio.ChannelCount];
        audio.Read<float>(samples, _sampleCount);
        
        // 只取第一个声道
        _lineRenderer.positionCount = _sampleCount;
        for (int i = 0; i < _sampleCount; i++)
        {
            float x = (float)i / _sampleCount;
            float y = samples[i * audio.ChannelCount];
            _lineRenderer.SetPosition(i, new Vector3(x, y, 0));
        }
    }
}
```

### 音频截取

```csharp
public float[] ExtractSegment(string path, float startSec, float durationSec)
{
    using var audio = new AudioAsset();
    audio.Load(path, SampleFormat.Float);
    
    long startFrame = (long)(startSec * audio.SampleRate);
    long frameCount = (long)(durationSec * audio.SampleRate);
    
    audio.Seek(startFrame);
    
    float[] buffer = new float[frameCount * audio.ChannelCount];
    audio.Read<float>(buffer, (int)frameCount);
    
    return buffer;
}
```
