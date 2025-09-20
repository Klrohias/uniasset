using System;
using System.Threading;

using Uniasset.Unsafe;

namespace Uniasset.Audio.Player
{
    public sealed class AudioEngine : IDisposable
    {
        private int _disposed;
        
        public UnsafeAudioEngine UnsafeHandler { get; } = UnsafeAudioEngine.Create();

        public float Volume
        {
            get => UnsafeHandler.GetVolume();
            set => UnsafeHandler.SetVolume(value);
        }

        public ulong TimeInPcmFrames => UnsafeHandler.GetTimeInPcmFrames();

        public uint SampleRate => UnsafeHandler.GetSampleRate();

        public void ResetTimeInPcmFrames()
        {
            UnsafeHandler.ResetTimeInPcmFrames();
        }

        public PlaybackInstance CreatePlaybackInstance(AudioDecoder decoder)
        {
            var playbackHandler = UnsafeHandler.CreatePlaybackInstance(decoder.UnsafeHandle);
            return new PlaybackInstance(playbackHandler);
        }
        
        public void Dispose()
        {
            if (Interlocked.CompareExchange(ref _disposed, 1, 0) != 0) return;
            UnsafeHandler.Destroy();
            GC.SuppressFinalize(this);
        }
        
        ~AudioEngine()
        {
            Dispose();
        }
    }
}