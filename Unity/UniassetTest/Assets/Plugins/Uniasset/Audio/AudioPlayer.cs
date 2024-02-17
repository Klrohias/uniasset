using System;
using Uniasset.Unsafe;

namespace Uniasset.Audio
{
    public sealed class AudioPlayer : IDisposable
    {
        private bool _disposed = false;
        public UnsafeAudioPlayer UnsafeHandle { get; } = UnsafeAudioPlayer.Create();

        public float Time
        {
            set => UnsafeHandle.SetTime(value);
            get => UnsafeHandle.GetTime();
        }

        public float Volume
        {
            set => UnsafeHandle.SetVolume(value);
            get => UnsafeHandle.GetVolume();
        }

        public bool Paused
        {
            set
            {
                if (value)
                    Pause();
                else
                    Resume();
            }
            get => UnsafeHandle.IsPaused();
        }

        public void Open(AudioAsset audioAsset)
        {
            UnsafeHandle.Open(audioAsset.UnsafeHandle);
        }

        public void Close()
        {
            UnsafeHandle.Close();
        }

        public void Resume()
        {
            UnsafeHandle.Resume();
        }

        public void Pause()
        {
            UnsafeHandle.Pause();
        }

        public void Dispose()
        {
            if (_disposed) return;
            _disposed = true;

            UnsafeHandle.Destroy();
        }

        ~AudioPlayer()
        {
            Dispose();
        }
    }
}
