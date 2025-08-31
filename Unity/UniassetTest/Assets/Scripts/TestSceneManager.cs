using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using Stopwatch = System.Diagnostics.Stopwatch;
using Uniasset.Audio;
using Uniasset.Audio.Player;
using Uniasset.Image;
using Uniasset.Unsafe;

using UnityEngine;
using UnityEngine.UI;

public class TestSceneManager : MonoBehaviour
{
    public RawImage display;
    public AudioSource source;

    private ImageAsset _imageAsset = new();
    private AudioAsset _audioAsset = new();

    private Rect _imageTestWindow;
    private Rect _audioTestWindow = new(100, 100, 100, 100);

    private byte[] _testJpg;
    private byte[] _testPng;
    private byte[] _testWebp;
    // private byte[] _test1080P;
    private byte[] _testMp3;
    private Rect _draggableRect = new(0, 0, 10000, 20);
    private byte[] _testOgg;
    private AudioEngine _engine;
    private readonly List<PlaybackInstance> _playbacks = new();

    private async void Start()
    {
        _testJpg = await Utils.LoadStreamingAsset("Test.jpg", this);
        _testPng = await Utils.LoadStreamingAsset("Test.png", this);
        _testWebp = await Utils.LoadStreamingAsset("Test.webp", this);
        // _test1080P = await Utils.LoadStreamingAsset("Large.png", this);

        _testMp3 = await Utils.LoadStreamingAsset("音乐.ogg", this);
        // _testOgg = await Utils.LoadStreamingAsset("click.ogg", this);
        // AudioClip.PCMSetPositionCallback

        _engine = new AudioEngine();
    }

    private void ImageTestWindow(int id)
    {
        GUI.DragWindow(_draggableRect);
        GUILayout.BeginHorizontal();

        if (GUILayout.Button("Load Jpg (Sync)")) LoadImageSync(_testJpg);

        if (GUILayout.Button("Load Png (Sync)")) LoadImageSync(_testPng);

        if (GUILayout.Button("Load WebP (Sync)")) LoadImageSync(_testWebp);

        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();

        if (GUILayout.Button("Load Jpg (Async)")) LoadImageAsync(_testJpg);

        if (GUILayout.Button("Load Png (Async)")) LoadImageAsync(_testPng);

        if (GUILayout.Button("Load WebP (Async)")) LoadImageAsync(_testWebp);

        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();

        if (GUILayout.Button("Clip (Async)")) ClipImageAsync();

        if (GUILayout.Button("ClipMulti[1] (Async)")) ClipImageMultiAsync();

        if (GUILayout.Button("Resize (Async)")) ResizeImageAsync();

        // if (GUILayout.Button("Load Large (Async)")) LoadImageAsync(_test1080P, false);

        if (GUILayout.Button("Unload")) _imageAsset.Unload();

        GUILayout.EndHorizontal();
    }

    private void AudioTestWindow(int id)
    {
        GUI.DragWindow(_draggableRect);
        GUILayout.BeginHorizontal();
        
        if (GUILayout.Button("Load")) _audioAsset.Load(_testMp3);

        if (GUILayout.Button("Load File"))
        {
            if (Application.platform == RuntimePlatform.Android)
                throw new PlatformNotSupportedException("Couldn't load streaming asset from file on Android.");
            
            var path = System.IO.Path.Combine(Application.streamingAssetsPath, "测试3.mp3");
            _audioAsset.Load(path);
            Debug.Log("Loaded Path: " + _audioAsset.Path);
        }

        if (GUILayout.Button("Load Adopt")) LoadAudioAdopt();
    
        if (GUILayout.Button("Unload")) _audioAsset.Unload();
    
        if (GUILayout.Button("Create playback"))
        {
            var instance = _engine.CreatePlaybackInstance(_audioAsset.GetAudioDecoder());
            _playbacks.Add(instance);
        }
        GUILayout.EndHorizontal();
    
        for (var i = _playbacks.Count - 1; i >= 0; i--)
        {
            if (_playbacks[i] != null) 
                continue;
            
            _playbacks.RemoveAt(i);
            for (var j = 0; j < 10; j++)
                GC.Collect();
        }
        for (var i = 0; i < _playbacks.Count; i++)
            DrawPlaybackControl(_playbacks[i]);
        
        GUILayout.BeginHorizontal();
        if (GUILayout.Button("AudioSourceOpen"))
        {
            var decoder = _audioAsset.GetAudioDecoder(frameBufferSize: _audioAsset.SampleRate * 64);
            var clip = decoder.ToAudioClip();
            source.clip = clip;
        }
    
        if (GUILayout.Button("AudioSourceOpen(Read)"))
        {
            var decoder = _audioAsset.GetAudioDecoder();
            var clip = AudioClip.Create(name, (int)decoder.SampleCount / decoder.ChannelCount
                , decoder.ChannelCount, decoder.SampleRate, false);
            var samples = new float[decoder.SampleCount];
            decoder.Read(new Span<float>(samples), (int)(decoder.SampleCount / decoder.ChannelCount));
            clip.SetData(samples, 0);
            source.clip = clip;
        }
        GUILayout.EndHorizontal();
    
        GUILayout.BeginHorizontal();
        if (GUILayout.Button("AudioSourcePause")) source.Pause();
        if (GUILayout.Button("AudioSourceResume")) source.Play();
        if (GUILayout.Button("AudioSourceClose")) source.clip = null;
        GUILayout.EndHorizontal();
    }

    private void DrawPlaybackControl(PlaybackInstance playback)
    {
        GUILayout.BeginHorizontal();
        if (playback.IsPlaying)
        {
            if (GUILayout.Button("Stop"))
                playback.Stop();
            if (GUILayout.Button("Stop +1s"))
                playback.StopScheduled(CalculateFrame(_engine, 1f));
        }
        else
        {
            if (GUILayout.Button("Play"))
                playback.Play();
            if (GUILayout.Button("Play +1s"))
            {
                playback.StopScheduled(ulong.MaxValue);
                playback.PlayScheduled(CalculateFrame(_engine, 1f));
            }
        }

        GUILayout.BeginVertical();
        var time = playback.Time;
        var newTime = GUILayout.HorizontalSlider(time, 0f, _audioAsset.Length);
        if (!Mathf.Approximately(newTime, time))
            playback.Time = newTime;
        GUILayout.Label($"{playback.Time:0.00} / {_audioAsset.Length:0.00} s");
        GUILayout.EndVertical();

        GUILayout.BeginVertical();
        var volume = playback.Volume;
        var newVolume = GUILayout.HorizontalSlider(volume, 0f, 1f);
        if (!Mathf.Approximately(newVolume, volume))
            playback.Volume = newVolume;
        GUILayout.Label($"Volume: {playback.Volume:0.00}");
        GUILayout.EndVertical();

        if (GUILayout.Button("Dispose"))
        {
            var index = _playbacks.IndexOf(playback);
            _playbacks[index] = null;
        }
        
        GUILayout.EndHorizontal();
    }

    private static ulong CalculateFrame(AudioEngine engine, float afterSeconds)
    {
        var nowFrame = engine.TimeInPcmFrames;
        var playFrame = engine.TimeInPcmFrames + (ulong)(engine.SampleRate * afterSeconds);
        Debug.Log($"SampleRate: {engine.SampleRate}, nowFrame: {nowFrame}, playFrame: {playFrame}, afterSeconds: {afterSeconds}");
        return playFrame;
    }

    private void OnGUI()
    {
        _imageTestWindow = GUILayout.Window(0, _imageTestWindow, ImageTestWindow, "Image");

        _audioTestWindow = GUILayout.Window(1, _audioTestWindow, AudioTestWindow, "Audio");
    }

    private void LoadImageSync(byte[] data, bool displaying = true)
    {
        var s = Stopwatch.StartNew();
        _imageAsset.Load(data);
        Debug.Log("Load: " + s.ElapsedMilliseconds + "ms");

        Debug.Log($"Width = {_imageAsset.Width}, Height = {_imageAsset.Height}, Channels = {_imageAsset.ChannelCount}");

        if (!displaying) return;
        s.Restart();
        display.texture = _imageAsset.ToTexture2D();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    private async void LoadImageAsync(byte[] data, bool displaying = true)
    {
        var s = Stopwatch.StartNew();
        await _imageAsset.LoadAsync(data);
        Debug.Log("Load: " + s.ElapsedMilliseconds + "ms");

        Debug.Log($"Width = {_imageAsset.Width}, Height = {_imageAsset.Height}, Channels = {_imageAsset.ChannelCount}");

        if (!displaying) return;
        s.Restart();
        display.texture = await _imageAsset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }


    private async void ResizeImageAsync()
    {
        var s = Stopwatch.StartNew();
        await _imageAsset.ResizeAsync(800, 800);
        Debug.Log("Resize: " + s.ElapsedMilliseconds + "ms");

        s.Restart();
        display.texture = await _imageAsset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    private async void ClipImageAsync()
    {
        var s = Stopwatch.StartNew();
        await _imageAsset.CropAsync(25, 25, 50, 50);
        Debug.Log("Clip: " + s.ElapsedMilliseconds + "ms");

        s.Restart();
        display.texture = await _imageAsset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    private async void ClipImageMultiAsync()
    {
        var s = Stopwatch.StartNew();
        var result = await _imageAsset.CropMultipleAsync(new[]
        {
            new CropOptions(0, 0, 25, 25),
            new CropOptions(25, 25, 50, 50),
            new CropOptions(50, 50, 25, 25),
        });
        Debug.Log("ClipMultiple: " + s.ElapsedMilliseconds + "ms");

        s.Restart();
        display.texture = await result[1].ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    private void LoadAudioAdopt()
    {
        var mem = NativeMemoryManager.Allocate(_testMp3.Length);
        _testMp3.AsSpan().CopyTo(mem.GetSpan());
        _audioAsset.LoadAdopt(mem);
    }
}