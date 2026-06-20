# Audio Loading Guide

This guide explains how to use `Uniasset.Audio.AudioAsset` to load audio, read PCM data, and convert it to a Unity `AudioClip`.

## Supported formats

Uniasset currently supports:

| Format | Extensions | Notes |
|------|--------|------|
| MP3 | `.mp3` | Common lossy format |
| FLAC | `.flac` | Lossless compression |
| WAV | `.wav` | Common uncompressed container |
| PCM | `.pcm` | Raw PCM data |
| Vorbis | — | Vorbis-encoded audio |
| OGG | `.ogg` | Common Vorbis container |

## Basic loading

### Load from file

```csharp
using Uniasset.Audio;

using var audio = new AudioAsset();
audio.Load("Assets/Audio/bgm.mp3");

Debug.Log($"Sample rate: {audio.SampleRate} Hz");
Debug.Log($"Channels: {audio.ChannelCount}");
Debug.Log($"Frame count: {audio.FrameCount}");
```

### Load from a byte array

```csharp
using var audio = new AudioAsset();

byte[] data = File.ReadAllBytes("sound.wav");
audio.Load(data);
```

### Load from a custom stream

```csharp
using Uniasset;
using Uniasset.Audio;

using var audio = new AudioAsset();
using var fileStream = File.OpenRead("music.flac");
var stream = new StreamWrapper(fileStream);

audio.LoadIO(stream);
```

!!! note
    `AudioAsset` currently exposes `LoadIO(IUniassetStream)` only. There is no `LoadIO(Stream)` convenience overload.

## Sample format

You can choose the output sample format during load:

```csharp
audio.Load("music.mp3", SampleFormat.Float);
audio.Load("music.mp3", SampleFormat.Int16);
```

Recommended defaults:

- Use `SampleFormat.Float` when handing data to Unity or doing floating-point processing
- Use `SampleFormat.Int16` when memory usage matters or when you need 16-bit PCM directly

See [SampleFormat API Reference](../api/sample-format.md) for details.

## Convert to `AudioClip`

### Streaming playback

```csharp
AudioClip clip = audio.ToAudioClip("BGM", stream: true);
audioSource.clip = clip;
audioSource.Play();
```

Good for:

- Background music
- Long-form audio
- Lower one-shot memory usage

### Fully loaded playback

```csharp
AudioClip clip = audio.ToAudioClip("Explosion", stream: false);
```

Good for:

- Short sound effects
- Frequently repeated clips

## Read PCM data manually

### Basic read

```csharp
using var audio = new AudioAsset();
audio.Load("music.flac", SampleFormat.Float);

float[] buffer = new float[1024 * audio.ChannelCount];
int framesRead = audio.Read<float>(buffer, 1024);
```

### Read the whole file in chunks

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

Debug.Log($"Read {totalFrames} frames in total");
```

### Read with `Int16`

```csharp
using var audio = new AudioAsset();
audio.Load("sound.wav", SampleFormat.Int16);

short[] buffer = new short[1024 * audio.ChannelCount];
int framesRead = audio.Read<short>(buffer, 1024);
```

!!! warning
    The type parameter `T` in `Read<T>(...)` must match the selected `SampleFormat`:
    `SampleFormat.Float -> float`, `SampleFormat.Int16 -> short`.

## Seeking

### Get the current position

```csharp
long currentFrame = audio.Tell();
Debug.Log($"Current frame: {currentFrame} / {audio.FrameCount}");
```

### Seek to a frame

```csharp
audio.Seek(1000);
audio.Seek(0);
```

### Convert between seconds and frames

```csharp
float seconds = (float)audio.Tell() / audio.SampleRate;

long frame = (long)(10.5f * audio.SampleRate);
audio.Seek(frame);
```

## Lifetime management

### Use `using`

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
AudioClip clip = audio.ToAudioClip();
```

### Unload loaded data

```csharp
audio.Unload();
```

`Unload()` clears the current audio data while keeping the instance reusable.

## Common use cases

### Background music player

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

### Sound-effect cache

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

### Waveform preview

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

### Extract a time range

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
