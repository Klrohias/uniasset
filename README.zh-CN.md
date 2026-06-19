[English](./README.md) | 中文

# Uniasset
![uniasset](https://socialify.git.ci/klrohias/uniasset/image?description=1&descriptionEditable=%E5%9C%A8%20Unity%20%E4%B8%AD%E6%96%B9%E4%BE%BF%E7%9A%84%E8%BD%BD%E5%85%A5%E5%92%8C%E4%BD%BF%E7%94%A8%E5%A4%96%E7%BD%AE%E8%B5%84%E6%BA%90&font=Raleway&forks=1&issues=1&language=1&name=1&owner=1&pattern=Signal&pulls=1&stargazers=1&theme=Auto)  

# 使用
## 安装
1. 进入 [Releases](https://github.com/Klrohias/uniasset/releases) 页面，下载最新版本中的 `uniasset-unity-scripts.zip`。
2. 将压缩包解压到 Unity 工程中。
3. 在 Unity Editor 中点击 `Tools > Uniasset > Download Native Libraries`。
4. 下载完成后，点击 `Tools > Uniasset > Configure Native Libraries`。

## 基本使用
```csharp
using System.Threading.Tasks;
using Uniasset.Image;
using Uniasset.Audio;

async Task LoadAssetsAsync(RawImage image, AudioSource audioSource)
{
    // 载入图像
    using var imageAsset = new ImageAsset();
    await imageAsset.LoadAsync("/path/to/your/image.png");

    // 裁切
    await imageAsset.CropAsync(100, 100, 100, 100);

    // 缩放
    await imageAsset.ResizeAsync(50, 50);

    // 转为 Texture2D 并显示
    image.texture = await imageAsset.ToTexture2DAsync();

    // 载入音频
    using var audioAsset = new AudioAsset();
    audioAsset.Load("/path/to/your/audio.ogg");

    // 转为 AudioClip 播放
    audioSource.clip = audioAsset.ToAudioClip("BGM", stream: true);
    audioSource.Play();

    await Task.Delay(5000);

    audioSource.Pause();
}

```

# 许可证
这个项目使用了 [MIT](LICENSE) 许可证，可以根据具体的项目需求（如资源加密等）进行修改，而无需重新开源。
