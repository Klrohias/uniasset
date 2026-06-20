# AudioAsset

`Uniasset.Audio.AudioAsset` is the high-level audio wrapper for loading audio, reading PCM frames sequentially, seeking arbitrarily, and converting the result to a Unity `AudioClip`.

## Definition

```csharp
namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable { }
}
```

## Read-only properties

| Property | Type | Description |
|------|------|------|
| `SampleRate` | `int` | Sample rate, for example `44100` |
| `SampleCount` | `long` | Total sample count |
| `ChannelCount` | `int` | Number of channels |
| `FrameCount` | `long` | Total frame count |

For multi-channel audio:

```csharp
SampleCount == FrameCount * ChannelCount
```

## Loading

### `Load`

```csharp
public void Load(string path, SampleFormat sampleFormat = SampleFormat.Float)
```

Loads audio from a file path.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `path` | `string` | — | Audio file path |
| `sampleFormat` | `SampleFormat` | `Float` | Decoded sample format |

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
```

### `Load(Span<byte>)`

```csharp
public void Load(Span<byte> data, SampleFormat sampleFormat = SampleFormat.Float)
```

Loads audio from a byte buffer.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `data` | `Span<byte>` | — | Encoded audio bytes |
| `sampleFormat` | `SampleFormat` | `Float` | Decoded sample format |

```csharp
byte[] audioData = File.ReadAllBytes("music.mp3");
audio.Load(audioData);
```

### `LoadIO`

```csharp
public void LoadIO(IUniassetStream stream, SampleFormat sampleFormat = SampleFormat.Float)
```

Loads audio from a custom stream.

If `stream` is `null`, the method throws `ArgumentNullException`.

!!! note
    `AudioAsset` currently has no `LoadAsync(...)` overloads and no `LoadIO(Stream)` convenience overload.

## Reading and seeking

### `Prepare`

```csharp
public void Prepare()
```

Fully decodes the audio into PCM data. After this call, the underlying decoder switches from streaming decode to in-memory PCM.

This method is a prerequisite for `TryClone` — a streaming-decoded audio asset must call `Prepare` before it can be cloned. If the asset is already in PCM mode (previously prepared or cloned), this method is a no-op.

| Exception | Condition |
|------|------|
| `NativeException` | IO error or unsupported format during decode, or asset not loaded |

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
audio.Prepare();  // fully decode

var clone = audio.TryClone();  // now cloneable
```

### `Tell`

```csharp
public long Tell()
```

Returns the current read position in frames.

```csharp
long position = audio.Tell();
Debug.Log($"Current position: {position} / {audio.FrameCount}");
```

### `Seek`

```csharp
public void Seek(long position)
```

Seeks to the specified frame index.

| Parameter | Type | Description |
|------|------|------|
| `position` | `long` | Target frame index |

```csharp
audio.Seek(1000);
```

### `Read<T>`

```csharp
public int Read<T>(Span<T> buffer, int frameCount) where T : unmanaged
```

Reads PCM frames into the destination buffer.

| Parameter | Type | Description |
|------|------|------|
| `buffer` | `Span<T>` | Destination buffer |
| `frameCount` | `int` | Number of frames to read |

The generic type `T` must match the selected `SampleFormat`:

| SampleFormat | T |
|--------------|---|
| `Float` | `float` |
| `Int16` | `short` |

```csharp
float[] buffer = new float[1024 * audio.ChannelCount];
int framesRead = audio.Read<float>(buffer, 1024);
```

The return value is the number of frames actually read.

### `SeekUnsafe`

```csharp
public void SeekUnsafe(long position)
```

Seeks to the specified frame position.

!!! warning "Thread safety"
    This method is not thread-safe. Use it only when the asset is accessed from a single thread. Use `Seek` in concurrent scenarios.

| Parameter | Type | Description |
|------|------|------|
| `position` | `long` | Target frame index |

It behaves like `Seek` but skips internal synchronization for performance.

```csharp
audio.SeekUnsafe(1000);
```

### `ReadUnsafe<T>`

```csharp
public int ReadUnsafe<T>(Span<T> buffer, int frameCount) where T : unmanaged
```

Reads PCM frames into the destination buffer.

!!! warning "Thread safety"
    This method is not thread-safe. Use it only when the asset is accessed from a single thread. Use `Read` in concurrent scenarios.

| Parameter | Type | Description |
|------|------|------|
| `buffer` | `Span<T>` | Destination buffer |
| `frameCount` | `int` | Number of frames to read |

The generic type `T` must match the selected `SampleFormat`:

| SampleFormat | T |
|--------------|---|
| `Float` | `float` |
| `Int16` | `short` |

It behaves like `Read<T>` but skips internal synchronization for performance.

```csharp
float[] buffer = new float[1024 * audio.ChannelCount];
int framesRead = audio.ReadUnsafe<float>(buffer, 1024);
```

The return value is the number of frames actually read.

## Conversion

### `TryClone`

```csharp
public AudioAsset TryClone()
```

Clones the current audio asset, returning a new `AudioAsset` instance. Both instances share the same PCM data (reference-counted) and maintain independent read positions.

**Precondition**: `Prepare()` must be called first. If the asset is still in streaming-decode mode, an exception is thrown.

| Exception | Condition |
|------|------|
| `NativeException` | `Prepare()` not called, or asset not loaded |

```csharp
using var audio = new AudioAsset();
audio.Load("music.mp3");
audio.Prepare();

var clone = audio.TryClone();
// audio and clone share PCM data, each with independent seek/read
clone.Seek(0);
```

### `ToAudioClip`

```csharp
public AudioClip ToAudioClip(string name = "created_from_uniasset", bool stream = true)
```

Converts the audio into a Unity `AudioClip`.

| Parameter | Type | Default | Description |
|------|------|--------|------|
| `name` | `string` | `"created_from_uniasset"` | AudioClip name |
| `stream` | `bool` | `true` | Whether to create a streaming clip |

**Streaming (`stream = true`):**

- Audio data is pulled on demand during playback
- Lower memory usage, suitable for long clips

**Fully loaded (`stream = false`):**

- Unity pulls the complete audio data during clip creation
- Better for short sound effects or repeated playback

```csharp
AudioClip bgm = audio.ToAudioClip("BGM", stream: true);
AudioClip sfx = audio.ToAudioClip("Explosion", stream: false);
```

## Lifetime

### `Unload`

```csharp
public void Unload()
```

Unloads the currently loaded audio data. You can call `Load` again afterward.

### `Dispose`

```csharp
public void Dispose()
```

Releases the underlying native resources. The instance should not be used afterward.

## Examples

### Load audio and play it

```csharp
using Uniasset.Audio;

using var audio = new AudioAsset();
audio.Load("background.mp3");

AudioClip clip = audio.ToAudioClip("BGM");
audioSource.clip = clip;
audioSource.Play();
```

### Read PCM data manually

```csharp
using var audio = new AudioAsset();
audio.Load("music.flac", SampleFormat.Float);
```
