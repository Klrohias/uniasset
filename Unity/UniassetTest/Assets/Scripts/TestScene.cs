using System;
using System.IO;
using Stopwatch = System.Diagnostics.Stopwatch;
using Uniasset;
using UnityEngine;
using UnityEngine.UI;

public class TestScene : MonoBehaviour
{
    private byte[] _azusaImageData;
    private byte[] _audioData;
    public RawImage display;
    private AudioPlayer _audioPlayer;
    private AudioAsset _audioAsset;

    private async void Start()
    {
        _azusaImageData = await Utils.LoadStreamingAsset("Azusa.png", this);
        _audioData = await File.ReadAllBytesAsync("/Users/mac/Documents/TestAudio.mp3");
    }
    private void OnGUI()
    {
        if (GUILayout.Button("LoadSync (UWR)")) LoadImageSync();
        if (GUILayout.Button("LoadAsync (UWR)")) LoadImageAsync();
        if (GUILayout.Button("ResizeAsync (UWR)")) ResizeImageAsync();
        if (GUILayout.Button("ClipAsync (UWR)")) ClipImageAsync();
        if (GUILayout.Button("Create Player")) CreatePlayer();
        if (GUILayout.Button("Create AudioAsset")) CreateAudioAsset();
        if (GUILayout.Button("Open Audio")) OpenAudio();
        if (GUILayout.Button("Pause Audio")) PauseAudio();
        if (GUILayout.Button("Resume Audio")) ResumeAudio();
        if (GUILayout.Button("Close Audio")) CloseAudio();
        if (GUILayout.Button("Get Player Time")) GetPlayerTime();
        
        if (GUILayout.Button("Lower volume"))
        {
            _audioPlayer.Volume = .4f;
        }
        
        if (GUILayout.Button("Normal volume"))
        {
            _audioPlayer.Volume = 1f;
        }
    }

    private void GetPlayerTime()
    {
        Debug.Log($"Time = {_audioPlayer.Time}");
    }

    private void CloseAudio()
    {
        _audioPlayer.Close();
    }

    private void OpenAudio()
    {
        _audioPlayer.Open(_audioAsset);
    }
    
    private void ResumeAudio()
    {
        _audioPlayer.Resume();
    }

    private void PauseAudio()
    {
        _audioPlayer.Pause();
    }

    private void CreatePlayer()
    {
        _audioPlayer = new AudioPlayer();
    }

    private void CreateAudioAsset()
    {
        _audioAsset = new AudioAsset();
        
        _audioAsset.Load(_audioData);
        Debug.Log(
            $"ChannelCount = {_audioAsset.ChannelCount}, SampleCount = {_audioAsset.SampleCount}, SampleRate = {_audioAsset.SampleRate}, Length = {_audioAsset.Length}");
    }

    private void LoadImageSync()
    {
        var s = Stopwatch.StartNew();
        using var asset = new ImageAsset();
        asset.Load(_azusaImageData);
        Debug.Log("Load: " + s.ElapsedMilliseconds + "ms");
        
        Debug.Log($"Width = {asset.Width}, Height = {asset.Height}, Channels = {asset.ChannelCount}");
        
        s.Restart();
        display.texture = asset.ToTexture2D();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    
    private async void LoadImageAsync()
    {
        var s = Stopwatch.StartNew();
        using var asset = new ImageAsset();
        await asset.LoadAsync(_azusaImageData);
        Debug.Log("Load: " + s.ElapsedMilliseconds + "ms");
        
        Debug.Log($"Width = {asset.Width}, Height = {asset.Height}, Channels = {asset.ChannelCount}");
        
        s.Restart();
        display.texture = await asset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    
    
    private async void ResizeImageAsync()
    {
        var s = Stopwatch.StartNew();
        using var asset = new ImageAsset();
        await asset.LoadAsync(_azusaImageData);
        Debug.Log("Load: " + s.ElapsedMilliseconds + "ms");
        
        Debug.Log($"Width = {asset.Width}, Height = {asset.Height}, Channels = {asset.ChannelCount}");
        
        
        s.Restart();
        await asset.ResizeAsync(400, 400);
        Debug.Log("Resize: " + s.ElapsedMilliseconds + "ms");
        
        s.Restart();
        display.texture = await asset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }
    
    private async void ClipImageAsync()
    {
        var s = Stopwatch.StartNew();
        using var asset = new ImageAsset();
        await asset.LoadAsync(_azusaImageData);
        Debug.Log("Load: " + s.ElapsedMilliseconds + "ms");
        
        Debug.Log($"Width = {asset.Width}, Height = {asset.Height}, Channels = {asset.ChannelCount}");
        
        
        s.Restart();
        await asset.ClipAsync(100, 100, 400, 400);
        Debug.Log("Clip: " + s.ElapsedMilliseconds + "ms");
        
        s.Restart();
        display.texture = await asset.ToTexture2DAsync();
        Debug.Log("ToTexture2D: " + s.ElapsedMilliseconds + "ms");
    }

    private void OnDestroy()
    {
        _audioAsset?.Dispose();
        _audioPlayer?.Dispose();
    }
}
