# Getting Started

## Installation

There is currently one installation path:

1. Download `uniasset-unity-scripts.zip` from [Releases](https://github.com/Klrohias/uniasset/releases)
2. Extract it into your Unity project
3. In the Unity Editor, click **Tools > Uniasset > Download Native Libraries**
4. After the download completes, click **Tools > Uniasset > Configure Native Libraries**

## Basic usage

### Load an image

```csharp
using Uniasset;
using Uniasset.Image;

var image = new ImageAsset();

// Load from file
image.Load("path/to/image.png");

// Read image metadata
Debug.Log($"Size: {image.Width} x {image.Height}");
Debug.Log($"Channels: {image.ChannelCount}");

// Convert to Unity Texture2D
Texture2D texture = image.ToTexture2D();

// Release resources when done
image.Dispose();
```

### Load audio

```csharp
using Uniasset;
using Uniasset.Audio;

var audio = new AudioAsset();

// Load from file
audio.Load("path/to/audio.mp3");

// Read audio metadata
Debug.Log($"Sample rate: {audio.SampleRate}");
Debug.Log($"Channels: {audio.ChannelCount}");
Debug.Log($"Frame count: {audio.FrameCount}");

// Convert to Unity AudioClip
AudioClip clip = audio.ToAudioClip();

// Release resources when done
audio.Dispose();
```

### Asynchronous APIs

All expensive operations also provide async variants to avoid blocking Unity's main thread:

```csharp
using Uniasset;
using Uniasset.Image;

var image = new ImageAsset();

// Load asynchronously
await image.LoadAsync("path/to/image.png");

// Crop asynchronously
await image.CropAsync(0, 0, 100, 100);

// Resize asynchronously
await image.ResizeAsync(200, 200);

// Convert asynchronously
Texture2D texture = await image.ToTexture2DAsync();

image.Dispose();
```

## System requirements

- Unity 2021.3 or later
- Supported platforms: Windows, macOS, Linux, Android, iOS

## Next steps

- [Image Loading Guide](guide/image-loading.md) for image decoding and processing
- [Audio Loading Guide](guide/audio-loading.md) for audio loading and PCM access
- [Custom Streams](guide/custom-streams.md) for arbitrary data sources
- [API Reference](api/index.md) for the complete public API
