using System;
using System.Threading;

using Uniasset.Unsafe;

namespace Uniasset.Audio.Player
{
    public class PlaybackInstance : IDisposable
    {
        private int _disposed;

        public UnsafePlaybackInstance UnsafeHandler { get; }

        public float Volume
        {
            get => UnsafeHandler.GetVolume();
            set => UnsafeHandler.SetVolume(value);
        }

        public float Time
        {
            get => UnsafeHandler.GetTime();
            set => UnsafeHandler.SetTime(value);
        }

        public ulong FrameTime
        {
            get => UnsafeHandler.GetFrameTime();
            set => UnsafeHandler.SetFrameTime(value);
        }

        public bool IsPlaying => UnsafeHandler.IsPlaying();

        public bool IsLooping
        {
            get => UnsafeHandler.IsLooping();
            set => UnsafeHandler.SetLooping(value);
        }

        public PlaybackInstance(UnsafePlaybackInstance unsafeHandler)
        {
            UnsafeHandler = unsafeHandler;
        }

        public void Play()
        {
            UnsafeHandler.Play();
        }

        public void Stop()
        {
            UnsafeHandler.Stop();
        }

        public void PlayScheduled(ulong frame)
        {
            UnsafeHandler.PlayScheduled(frame);
        }

        public void StopScheduled(ulong frame)
        {
            UnsafeHandler.StopScheduled(frame);
        }

        public void Dispose()
        {
            if (Interlocked.CompareExchange(ref _disposed, 1, 0) != 0) return;
            UnsafeHandler.Destroy();
            GC.SuppressFinalize(this);
        }

        ~PlaybackInstance()
        {
            Dispose();
        }
    }
}