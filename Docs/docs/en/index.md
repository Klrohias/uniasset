---
title: Uniasset
hide:
  - navigation
  - toc
---

<div class="landing-hero" markdown>

# Uniasset

<p class="hero-subtitle">High-performance external asset loading for Unity</p>
<p class="hero-desc">Built on native Rust libraries, Uniasset brings asynchronous and efficient image and audio loading to Unity projects.</p>

<div class="hero-actions" markdown>

[Get Started](getting-started.md){ .btn-primary }
[GitHub :fontawesome-brands-github:](https://github.com/Klrohias/uniasset){ .btn-secondary }

</div>

<div class="hero-badges" markdown>

<span class="badge">:material-scale-balance: MIT</span>
<span class="badge">:material-language-csharp: C# + Rust</span>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## Why Unity asset loading is painful

External asset loading in Unity often runs into the same bottlenecks. Uniasset is designed to remove them.

</div>

<div class="pain-grid" markdown>

<div class="pain-card" markdown>
<div class="pain-icon" markdown>:material-turtle:</div>
<div markdown>

#### Slow processing

Cropping or resizing textures usually means multiple `SetPixels` passes, extra copies, and format conversions.

</div>
</div>

<div class="pain-card" markdown>
<div class="pain-icon" markdown>:material-file-cancel-outline:</div>
<div markdown>

#### Limited format support

Unity does not handle formats like WebP or FLAC well without extra tooling.

</div>
</div>

<div class="pain-card" markdown>
<div class="pain-icon" markdown>:material-cogs:</div>
<div markdown>

#### Inflexible data sources

Loading from network streams, encrypted packages, or custom storage layers is awkward.

</div>
</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## Core features

</div>

<div class="feature-grid" markdown>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-rocket-launch:</div>

### Performance first

The core is written in Rust and exposed through P/Invoke FFI, making full use of native performance and SIMD-friendly paths.

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-sync:</div>

### Async by default

Expensive operations expose `async/await` APIs that fit naturally into Unity coroutines and Tasks without blocking the main thread.

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-image-multiple:</div>

### Multi-format images

Supports JPEG, WebP, PNG, BMP, TGA, PSD, with built-in cropping and resizing.

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-music-circle:</div>

### Multi-format audio

Covers MP3, FLAC, WAV, PCM, Vorbis, OGG, and AAC decoding.

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-puzzle:</div>

### Custom data sources

Implement `IUniassetStream` to load from any source: network streams, encrypted bundles, AssetBundles, or memory caches.

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-folder-download:</div>

### One-step integration

Download `uniasset-unity-scripts.zip` from [Releases](https://github.com/Klrohias/uniasset/releases), then use the editor menu to fetch and configure native libraries.

</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## Architecture

</div>

<div class="how-works-flow" markdown>

<div class="how-step" markdown>
<div class="step-number" markdown>1</div>
<div class="step-label" markdown>C# SDK</div>
<div class="step-desc" markdown>A fully asynchronous C# API layer with high-level abstractions such as `ImageAsset` and `AudioAsset`.</div>
</div>

<div class="how-connector" markdown>:material-arrow-right:</div>

<div class="how-step" markdown>
<div class="step-number" markdown>2</div>
<div class="step-label" markdown>P/Invoke FFI</div>
<div class="step-desc" markdown>A lightweight interop layer that talks directly to native code with no extra serialization overhead.</div>
</div>

<div class="how-connector" markdown>:material-arrow-right:</div>

<div class="how-step" markdown>
<div class="step-number" markdown>3</div>
<div class="step-label" markdown>Rust Native Library</div>
<div class="step-desc" markdown>A high-performance Rust core responsible for decoding, processing, and memory management across platforms.</div>
</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## Supported formats

</div>

<div class="format-grid" markdown>

<div class="format-card" markdown>

### :material-image-outline: Images

- JPEG
- WebP (first frame only)
- PNG
- BMP
- TGA
- PSD
- GIF (first frame only)

</div>

<div class="format-card" markdown>

### :material-music-note: Audio

- MP3
- FLAC
- WAV
- Vorbis (OGG)
- AAC

</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## Code samples

</div>

<div class="code-preview" markdown>

<div class="code-block" markdown>

<div class="code-header" markdown>:material-image: Load and process an image</div>

```csharp
using Uniasset;
using Uniasset.Image;

using var image = new ImageAsset();
await image.LoadAsync("photo.png");

// Crop and resize
await image.CropAsync(0, 0, 256, 256);
await image.ResizeAsync(128, 128);

// Convert to Texture2D
Texture2D texture = await image.ToTexture2DAsync();
```

</div>

<div class="code-block" markdown>

<div class="code-header" markdown>:material-music: Load and play audio</div>

```csharp
using Uniasset;
using Uniasset.Audio;

using var audio = new AudioAsset();
audio.Load("music.mp3");

// Inspect audio metadata
Debug.Log($"Sample rate: {audio.SampleRate} Hz");
Debug.Log($"Channels: {audio.ChannelCount}");

// Convert to AudioClip
AudioClip clip = audio.ToAudioClip();
```

</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## Integrate in three steps

</div>

<div class="steps-grid" markdown>

<div class="step-card" markdown>
<div class="step-badge" markdown>1</div>

### Install the package

Download `uniasset-unity-scripts.zip` from [Releases](https://github.com/Klrohias/uniasset/releases) and extract it into your project.

</div>

<div class="step-card" markdown>
<div class="step-badge" markdown>2</div>

### Install native libraries

In the Unity Editor, run:

```text
Tools > Uniasset > Download Native Libraries
Tools > Uniasset > Configure Native Libraries
```

</div>

<div class="step-card" markdown>
<div class="step-badge" markdown>3</div>

### Start loading assets

Import the namespace and begin loading:

```csharp
using Uniasset;
using Uniasset.Image;

using var image = new ImageAsset();
await image.LoadAsync("assets/photo.png");
```

</div>

</div>

</div>

---

<div class="landing-cta" markdown>

## Ready to upgrade your Unity project?

<p>Add high-performance, asynchronous, multi-format external asset loading to your game.</p>

[Start with the docs :material-arrow-right:](getting-started.md){ .cta-btn }

</div>
