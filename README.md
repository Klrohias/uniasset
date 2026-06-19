[English] | [中文](README.zh-CN.md)

# Uniasset
![uniasset](https://socialify.git.ci/klrohias/uniasset/image?description=1&font=Raleway&forks=1&issues=1&language=1&name=1&owner=1&pattern=Signal&pulls=1&stargazers=1&theme=Auto)  

# Usage
## Installation
1. Navigate to the [Releases](https://github.com/Klrohias/uniasset/releases) page and download `uniasset-unity-scripts.zip` from the latest release.
2. Extract it into your Unity project.
3. In Unity Editor, click `Tools > Uniasset > Download Native Libraries`.
4. After the download finishes, click `Tools > Uniasset > Configure Native Libraries`.

## Basic Usage
```csharp
using System.Threading.Tasks;
using Uniasset.Image;
using Uniasset.Audio;

async Task LoadAssetsAsync(RawImage image, AudioSource audioSource)
{
    // Load image
    using var imageAsset = new ImageAsset();
    await imageAsset.LoadAsync("/path/to/your/image.png");

    // Crop
    await imageAsset.CropAsync(100, 100, 100, 100);

    // Resize
    await imageAsset.ResizeAsync(50, 50);

    // Convert to Texture2D and display
    image.texture = await imageAsset.ToTexture2DAsync();

    // Load audio
    using var audioAsset = new AudioAsset();
    audioAsset.Load("/path/to/your/audio.ogg");

    // Convert to AudioClip and play
    audioSource.clip = audioAsset.ToAudioClip("BGM", stream: true);
    audioSource.Play();

    await Task.Delay(5000);

    audioSource.Pause();
}
```

# License
This project is licensed under the [MIT License](LICENSE), which allows modification according to specific project requirements (such as resource encryption) without the need for re-licensing.
