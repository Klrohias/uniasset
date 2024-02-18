[English] | [中文](README.zh-CN.md)

# Uniasset
![uniasset](https://socialify.git.ci/klrohias/uniasset/image?description=1&font=Raleway&forks=1&issues=1&language=1&name=1&owner=1&pattern=Signal&pulls=1&stargazers=1&theme=Auto)  

# Usage
## Installation
1. Navigate to the [Releases](https://github.com/Klrohias/uniasset/releases) page, locate the latest release, and download the `uniasset-unity-scripts.zip` file from the assets provided in the release.
2. Depending on your project's requirements, download the corresponding native libraries (`.dll` for Windows, `.so` for Linux / Android, `.a` for iOS, `.dylib` for macOS) and place them in the `Assets/Plugins` directory of your Unity project. Configure the platform of the native libraries in the Inspector.

## Basic Usage
```csharp
using System.IO;
using System.Threading.Tasks;
using Uniasset.Image;
using Uniasset.Audio;

async void LoadAsync()
{
    // Load image
    var pathToResource = "/path/to/your/image.png";
    var fileContent = await File.ReadAllBytesAsync(pathToResource);

    using var imageAsset = new ImageAsset();
    await imageAsset.LoadAsync(fileContent);

    // Clip
    await imageAsset.ClipAsync(100, 100, 100, 100);

    // Resize
    await imageAsset.ResizeAsync(50, 50);

    // Convert to Texture2D and display
    // Note that although this is Async, due to Unity restrictions, some code still executes on the main thread, so make sure to call this on the main thread
    image.texture = await imageAsset.ToTexture2DAsync();

    // Load audio
    pathToResource = "/path/to/your/audio.ogg";
    fileContent = await File.ReadAllBytesAsync(pathToResource);

    using var audioAsset = new AudioAsset();
    await audioAsset.LoadAsync(fileContent);

    // Convert to AudioClip and play
    using var decoder = audioAsset.GetAudioDecoder();
    audioSource.clip = decoder.ToAudioClip();
    audioSource.Play();

    await Task.Delay(5000);

    audioSource.Pause();

    // Or use the provided AudioPlayer for playback
    using var player = new AudioPlayer();
    player.Open(audioAsset);
    player.Resume();

    await Task.Delay(5000);

    player.Pause();
}
```

# License
This project is licensed under the [MIT License](LICENSE), which allows modification according to specific project requirements (such as resource encryption) without the need for re-licensing.