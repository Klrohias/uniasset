---
title: Uniasset
hide:
  - navigation
  - toc
---

<div class="landing-hero" markdown>

# Uniasset

<p class="hero-subtitle">高性能 Unity 外部资源加载插件</p>
<p class="hero-desc">基于 Rust 原生库，为 Unity 提供异步、高效的外部图片与音频加载能力。</p>

<div class="hero-actions" markdown>

[快速开始](getting-started.md){ .btn-primary }
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

## Unity 资源加载的痛点

在 Unity 项目开发中，外部资源加载往往会遇到很多问题。Uniasset 专为解决这些痛点而生。

</div>

<div class="pain-grid" markdown>

<div class="pain-card" markdown>
<div class="pain-icon" markdown>:material-turtle:</div>
<div markdown>

#### 执行效率低

若需要对 Texture 进行缩放裁切，需要自行 `SetPixels`，经过多轮复制、转换。

</div>
</div>

<div class="pain-card" markdown>
<div class="pain-icon" markdown>:material-file-cancel-outline:</div>
<div markdown>

#### 格式支持有限

Unity 原生支持的格式不足，处理 WebP、FLAC 等格式需要额外方案。

</div>
</div>

<div class="pain-card" markdown>
<div class="pain-icon" markdown>:material-cogs:</div>
<div markdown>

#### 缺乏灵活的数据源

无法方便地从网络流、加密包或自定义存储层加载资源。

</div>
</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## 核心特性

</div>

<div class="feature-grid" markdown>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-rocket-launch:</div>

### 极致性能

底层由 Rust 编写，通过 P/Invoke FFI 直接调用原生代码，充分利用 SIMD 等加速。

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-sync:</div>

### 异步优先

所有耗时操作均提供 `async/await` 异步 API，天然适配 Unity 的协程与 Task 体系，告别主线程阻塞。

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-image-multiple:</div>

### 多格式图片

支持 JPEG、WebP、PNG、BMP、TGA、PSD，内置裁剪、缩放等图像处理方法。

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-music-circle:</div>

### 多格式音频

覆盖 MP3、FLAC、WAV、PCM、Vorbis、OGG 等音频格式。

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-puzzle:</div>

### 自定义数据源

实现 `IUniassetStream` 接口，即可从任意来源加载资源 — 网络流、加密包、AssetBundle、内存缓存。

</div>

<div class="feature-card" markdown>
<div class="feature-icon" markdown>:material-folder-download:</div>

### 一键集成

下载 [Release](https://github.com/Klrohias/uniasset/releases) 中的 `uniasset-unity-scripts.zip`，再通过编辑器菜单下载并配置原生库。

</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## 架构设计

</div>

<div class="how-works-flow" markdown>

<div class="how-step" markdown>
<div class="step-number" markdown>1</div>
<div class="step-label" markdown>C# SDK</div>
<div class="step-desc" markdown>完全异步的 C# API 层，提供 `ImageAsset`、`AudioAsset` 等高级抽象。</div>
</div>

<div class="how-connector" markdown>:material-arrow-right:</div>

<div class="how-step" markdown>
<div class="step-number" markdown>2</div>
<div class="step-label" markdown>P/Invoke FFI</div>
<div class="step-desc" markdown>轻量级跨语言调用层，C# 通过平台调用直接与原生库交互，零中间层、零序列化开销。</div>
</div>

<div class="how-connector" markdown>:material-arrow-right:</div>

<div class="how-step" markdown>
<div class="step-number" markdown>3</div>
<div class="step-label" markdown>Rust 原生库</div>
<div class="step-desc" markdown>高性能 Rust 核心，负责编解码、数据处理与内存管理。编译为各平台原生二进制，安全且高效。</div>
</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## 支持格式

</div>

<div class="format-grid" markdown>

<div class="format-card" markdown>

### :material-image-outline: 图片

- JPEG
- WebP (仅首帧)
- PNG
- BMP
- TGA
- PSD
- GIF (仅首帧)

</div>

<div class="format-card" markdown>

### :material-music-note: 音频

- MP3
- FLAC
- WAV
- Vorbis (OGG)

</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## 代码示例

</div>

<div class="code-preview" markdown>

<div class="code-block" markdown>

<div class="code-header" markdown>:material-image: 加载并处理图片</div>

```csharp
using Uniasset;
using Uniasset.Image;

using var image = new ImageAsset();
await image.LoadAsync("photo.png");

// 裁剪 & 缩放
await image.CropAsync(0, 0, 256, 256);
await image.ResizeAsync(128, 128);

// 转换为 Texture2D
Texture2D texture = await image.ToTexture2DAsync();
```

</div>

<div class="code-block" markdown>

<div class="code-header" markdown>:material-music: 加载并播放音频</div>

```csharp
using Uniasset;
using Uniasset.Audio;

using var audio = new AudioAsset();
audio.Load("music.mp3");

// 获取音频信息
Debug.Log($"采样率: {audio.SampleRate} Hz");
Debug.Log($"声道数: {audio.ChannelCount}");

// 转换为 AudioClip 播放
AudioClip clip = audio.ToAudioClip();
```

</div>

</div>

</div>

---

<div class="landing-section" markdown>

<div class="section-header" markdown>

## 三步集成

</div>

<div class="steps-grid" markdown>

<div class="step-card" markdown>
<div class="step-badge" markdown>1</div>

### 安装包

下载 [Release](https://github.com/Klrohias/uniasset/releases) 中的 `uniasset-unity-scripts.zip`，解压到项目目录中。

</div>

<div class="step-card" markdown>
<div class="step-badge" markdown>2</div>

### 安装原生库

在 Unity Editor 中依次点击以下菜单：

```text
Tools > Uniasset > Download Native Libraries
Tools > Uniasset > Configure Native Libraries
```

</div>

<div class="step-card" markdown>
<div class="step-badge" markdown>3</div>

### 开始使用

在脚本顶部添加命名空间引用后，即可开始加载资源：

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

## 准备好提升你的 Unity 项目了吗？

<p>为你的项目添加高性能、异步、多格式的外部资源加载能力。</p>

[阅读文档开始使用 :material-arrow-right:](getting-started.md){ .cta-btn }

</div>
