# AudioAsset

`Uniasset.Audio.AudioAsset` 是音频资源的高层封装，支持加载、顺序读取 PCM 数据、随机定位以及转换为 Unity `AudioClip`。

## 类定义

```csharp
namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable { }
}
```

## 只读属性

| 属性 | 类型 | 说明 |
|------|------|------|
| `SampleRate` | `int` | 采样率，例如 `44100` |
| `SampleCount` | `long` | 总采样点数 |
| `ChannelCount` | `int` | 声道数 |
| `FrameCount` | `long` | 总帧数 |

对多声道音频有：

```csharp
SampleCount == FrameCount * ChannelCount
```

## 加载

### Load

```csharp
public void Load(string path, SampleFormat sampleFormat = SampleFormat.Float)
```

从文件路径加载音频。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `path` | `string` | — | 音频文件路径 |
| `sampleFormat` | `SampleFormat` | `Float` | 解码输出的采样格式 |

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
```

### Load(Span<byte>)

```csharp
public void Load(Span<byte> data, SampleFormat sampleFormat = SampleFormat.Float)
```

从字节数组加载音频。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `data` | `Span<byte>` | — | 音频数据 |
| `sampleFormat` | `SampleFormat` | `Float` | 解码输出的采样格式 |

```csharp
byte[] audioData = File.ReadAllBytes("music.mp3");
audio.Load(audioData);
```

### LoadIO

```csharp
public void LoadIO(IUniassetStream stream, SampleFormat sampleFormat = SampleFormat.Float)
```

从自定义流加载音频。

`stream` 为 `null` 时会抛出 `ArgumentNullException`。

!!! note
    当前 `AudioAsset` 不提供 `LoadAsync(...)` 系列重载，也不提供 `LoadIO(Stream)` 便捷重载。

## 读取与定位

### Tell

```csharp
public long Tell()
```

获取当前读取位置，单位为帧。

```csharp
long position = audio.Tell();
Debug.Log($"当前位置: {position} / {audio.FrameCount}");
```

### Seek

```csharp
public void Seek(long position)
```

跳转到指定帧位置。

| 参数 | 类型 | 说明 |
|------|------|------|
| `position` | `long` | 目标帧位置 |

```csharp
audio.Seek(1000);
```

### Read<T>

```csharp
public int Read<T>(Span<T> buffer, int frameCount) where T : unmanaged
```

读取 PCM 帧数据到缓冲区。

| 参数 | 类型 | 说明 |
|------|------|------|
| `buffer` | `Span<T>` | 目标缓冲区 |
| `frameCount` | `int` | 要读取的帧数 |

类型参数 `T` 应与加载时指定的 `SampleFormat` 匹配：

| SampleFormat | T |
|--------------|---|
| `Float` | `float` |
| `Int16` | `short` |

```csharp
float[] buffer = new float[1024 * audio.ChannelCount];
int framesRead = audio.Read<float>(buffer, 1024);
```

返回值为实际读取的帧数。

### SeekUnsafe

```csharp
public void SeekUnsafe(long position)
```

跳转到指定帧位置。

!!! warning "线程安全性"
    该函数不保证线程安全，应在只有单个线程会访问该资源的场景中使用。多线程场景请使用 `Seek` 方法。

| 参数 | 类型 | 说明 |
|------|------|------|
| `position` | `long` | 目标帧位置 |

该方法与 `Seek` 功能相同，但跳过内部同步机制以提升性能。仅在确认不存在并发访问时使用。

```csharp
// 单线程场景下的使用示例
audio.SeekUnsafe(1000);
```

### ReadUnsafe<T>

```csharp
public int ReadUnsafe<T>(Span<T> buffer, int frameCount) where T : unmanaged
```

读取 PCM 帧数据到缓冲区。

!!! warning "线程安全性"
    该函数不保证线程安全，应在只有单个线程会访问该资源的场景中使用。多线程场景请使用 `Read` 方法。

| 参数 | 类型 | 说明 |
|------|------|------|
| `buffer` | `Span<T>` | 目标缓冲区 |
| `frameCount` | `int` | 要读取的帧数 |

类型参数 `T` 应与加载时指定的 `SampleFormat` 匹配：

| SampleFormat | T |
|--------------|---|
| `Float` | `float` |
| `Int16` | `short` |

该方法与 `Read<T>` 功能相同，但跳过内部同步机制以提升性能。仅在确认不存在并发访问时使用。

```csharp
// 单线程场景下的使用示例
float[] buffer = new float[1024 * audio.ChannelCount];
int framesRead = audio.ReadUnsafe<float>(buffer, 1024);
```

返回值为实际读取的帧数。

## 转换

### ToAudioClip

```csharp
public AudioClip ToAudioClip(string name = "created_from_uniasset", bool stream = true)
```

将音频转换为 Unity `AudioClip`。

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `name` | `string` | `"created_from_uniasset"` | AudioClip 名称 |
| `stream` | `bool` | `true` | 是否使用流式播放 |

**流式播放 (`stream = true`)：**

- 音频数据在播放时按需读取
- 内存占用低，适合长音频

**全量加载 (`stream = false`)：**

- Unity 会在创建时拉取完整数据
- 更适合短音效或需要快速重复播放的内容

```csharp
AudioClip bgm = audio.ToAudioClip("BGM", stream: true);
AudioClip sfx = audio.ToAudioClip("Explosion", stream: false);
```

## 生命周期

### Unload

```csharp
public void Unload()
```

卸载已加载的音频数据。卸载后可以重新调用 `Load` 方法加载新音频。

### Dispose

```csharp
public void Dispose()
```

释放底层资源。释放后不应继续使用该实例。

## 示例

### 加载音频并播放

```csharp
using Uniasset.Audio;

using var audio = new AudioAsset();
audio.Load("background.mp3");

AudioClip clip = audio.ToAudioClip("BGM");
audioSource.clip = clip;
audioSource.Play();
```

### 手动读取 PCM 数据

```csharp
using var audio = new AudioAsset();
audio.Load("music.flac", SampleFormat.Float);

float[] buffer = new float[4096 * audio.ChannelCount];
int totalFramesRead = 0;

while (true)
{
    int framesRead = audio.Read<float>(buffer, buffer.Length / audio.ChannelCount);
    if (framesRead == 0) break;

    totalFramesRead += framesRead;
}
```

### 使用 Int16 格式

```csharp
using var audio = new AudioAsset();
audio.Load("sound.wav", SampleFormat.Int16);

short[] buffer = new short[1024 * audio.ChannelCount];
int framesRead = audio.Read<short>(buffer, 1024);
```
