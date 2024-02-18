[English](./README.md) | 中文

# Uniasset
![uniasset](https://socialify.git.ci/klrohias/uniasset/image?description=1&descriptionEditable=%E5%9C%A8%20Unity%20%E4%B8%AD%E6%96%B9%E4%BE%BF%E7%9A%84%E8%BD%BD%E5%85%A5%E5%92%8C%E4%BD%BF%E7%94%A8%E5%A4%96%E7%BD%AE%E8%B5%84%E6%BA%90&font=Raleway&forks=1&issues=1&language=1&name=1&owner=1&pattern=Signal&pulls=1&stargazers=1&theme=Auto)  

# 使用
## 安装
1. 进入 [发行（Releases）](https://github.com/Klrohias/uniasset/releases) 页面，找到最新的发行版，在发行版提供的文件中找到 `uniasset-unity-scripts.zip` 文件，下载并将其解压到 Unity 工程中。
2. 根据项目的需求，下载对应的本机库（ `.dll` 为 Windows 的本机库，`.so` 为 Linux / Android 的本机库， `.a` 为 iOS 的本机库， `.dylib` 为 macOS 的本机库）放置到工程的 `Assets/Plugins` 路径内，在 Inspector 中配置好本机库的平台。

## 基本使用
```csharp
using System.IO;
using System.Threading.Tasks;
using Uniasset.Image;
using Uniasset.Audio;

async void LoadAsync()
{
    // 载入图像
    var pathToResource = "/path/to/your/image.png";
    var fileContent = await File.ReadAllBytesAsync(pathToResource);

    using var imageAsset = new ImageAsset();
    await imageAsset.LoadAsync(fileContent);

    // 裁切
    await imageAsset.ClipAsync(100, 100, 100, 100);

    // 缩放
    await imageAsset.ResizeAsync(50, 50);

    // 转为 Texture2D 并显示
    // 注意，尽管这里是 Async，但受限于 Unity，仍有部分代码在主线程执行，所以请务必在调用时确定在主线程调用
    image.texture = await imageAsset.ToTexture2DAsync();

    // 载入音频
    pathToResource = "/path/to/your/audio.ogg";
    fileContent = await File.ReadAllBytesAsync(pathToResource);

    using var audioAsset = new AudioAsset();
    await audioAsset.LoadAsync(fileContent);

    // 转为 AudioClip 播放
    using var decoder = audioAsset.GetAudioDecoder();
    audioSource.clip = decoder.ToAudioClip();
    audioSource.play();

    await Task.Delay(5000);

    audioSource.pause();

    // 或者使用提供的 AudioPlayer 播放
    using var player = new AudioPlayer();
    player.Open(audioAsset);
    player.Resume();

    await Task.Delay(5000);

    player.Pause();
}

```

# 许可证
这个项目使用了 [MIT](LICENSE) 许可证，可以根据具体的项目需求（如资源加密等）进行修改，而无需重新开源。
