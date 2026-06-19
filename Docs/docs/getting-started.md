# 快速开始

## 安装

唯一的安装方式：

1. 下载 [Release](https://github.com/Klrohias/uniasset/releases) 中的 `uniasset-unity-scripts.zip`
2. 解压到项目目录中
3. 在 Unity Editor 中，依次点击 **Tools > Uniasset > Download Native Libraries**
4. 下载完成后，依次点击 **Tools > Uniasset > Configure Native Libraries**

## 基本用法

### 加载图片

```csharp
using Uniasset;
using Uniasset.Image;

// 创建 ImageAsset 实例
var image = new ImageAsset();

// 从文件加载
image.Load("path/to/image.png");

// 获取图片信息
Debug.Log($"尺寸: {image.Width} x {image.Height}");
Debug.Log($"通道数: {image.ChannelCount}");

// 转换为 Unity Texture2D
Texture2D texture = image.ToTexture2D();

// 用完后释放资源
image.Dispose();
```

### 加载音频

```csharp
using Uniasset;
using Uniasset.Audio;

// 创建 AudioAsset 实例
var audio = new AudioAsset();

// 从文件加载
audio.Load("path/to/audio.mp3");

// 获取音频信息
Debug.Log($"采样率: {audio.SampleRate}");
Debug.Log($"通道数: {audio.ChannelCount}");
Debug.Log($"总帧数: {audio.FrameCount}");

// 转换为 Unity AudioClip
AudioClip clip = audio.ToAudioClip();

// 用完后释放资源
audio.Dispose();
```

### 异步加载

所有耗时操作都提供了异步版本，避免阻塞主线程：

```csharp
using Uniasset;
using Uniasset.Image;

var image = new ImageAsset();

// 异步加载图片
await image.LoadAsync("path/to/image.png");

// 异步裁剪
await image.CropAsync(0, 0, 100, 100);

// 异步缩放
await image.ResizeAsync(200, 200);

// 异步转换为 Texture2D
Texture2D texture = await image.ToTexture2DAsync();

image.Dispose();
```

## 系统要求

- Unity 2021.3 或更高版本
- 支持的平台：Windows、macOS、Linux、Android、iOS

## 下一步

- [图片加载指南](guide/image-loading.md) — 深入了解图片加载与处理
- [音频加载指南](guide/audio-loading.md) — 深入了解音频加载与处理
- [自定义流](guide/custom-streams.md) — 从自定义数据源加载资源
- [API 参考](api/index.md) — 查看完整的 API 文档
