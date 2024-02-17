using System;
using Stopwatch = System.Diagnostics.Stopwatch;
using Uniasset.Audio;
using Uniasset.Image;
using UnityEngine;
using UnityEngine.UI;

public class TestSceneManager : MonoBehaviour
{
    public RawImage display;
    public AudioSource source;

    private ImageAsset _imageAsset = new();
    private AudioAsset _audioAsset = new();
    private AudioPlayer _audioPlayer = new();

    private Rect _imageTestWindow;
    private Rect _audioTestWindow = new(100, 100, 100, 100);

    private byte[] _testJpg;
    private byte[] _testPng;
    private byte[] _testWebp;
    private byte[] _test1080P;
    private byte[] _testMp3;
    private Rect _draggableRect = new(0, 0, 10000, 20);
    private byte[] _testOgg;

    private async void Start()
    {
        _testJpg = await Utils.LoadStreamingAsset("Test.jpg", this);
        _testPng = await Utils.LoadStreamingAsset("Test.png", this);
        _testWebp = await Utils.LoadStreamingAsset("Test.webp", this);
        _test1080P = await Utils.LoadStreamingAsset("Large.png", this);

        _testMp3 = await Utils.LoadStreamingAsset("Test.mp3", this);
        _testOgg = await Utils.LoadStreamingAsset("click.ogg", this);
        // AudioClip.PCMSetPositionCallback
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

        if (GUILayout.Button("Resize (Async)")) ResizeImageAsync();

        if (GUILayout.Button("Load Large (Async)")) LoadImageAsync(_test1080P, false);

        if (GUILayout.Button("Unload")) _imageAsset.Unload();

        GUILayout.EndHorizontal();
    }

    private void AudioTestWindow(int id)
    {
        GUI.DragWindow(_draggableRect);
        GUILayout.BeginHorizontal();

        if (GUILayout.Button("Load")) _audioAsset.Load(_testOgg);

        if (GUILayout.Button("Unload")) _audioAsset.Unload();

        if (GUILayout.Button("AudioPlayerOpen")) _audioPlayer.Open(_audioAsset);


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
        if (GUILayout.Button("AudioPlayerPause")) _audioPlayer.Pause();
        if (GUILayout.Button("AudioPlayerResume")) _audioPlayer.Resume();
        if (GUILayout.Button("AudioPlayerClose")) _audioPlayer.Close();
        GUILayout.EndHorizontal();


        GUILayout.BeginHorizontal();
        if (GUILayout.Button("AudioSourcePause")) source.Pause();
        if (GUILayout.Button("AudioSourceResume")) source.Play();
        if (GUILayout.Button("AudioSourceClose")) source.clip = null;
        GUILayout.EndHorizontal();
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
        await _imageAsset.ClipAsync(25, 25, 50, 50);
        Debug.Log("Clip: " + s.ElapsedMilliseconds + "ms");

        s.Restart();
        display.texture = await _imageAsset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }
}