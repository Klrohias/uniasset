using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Uniasset.Unsafe;
using UnityEngine;

namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable
    {
        private int _disposed;
        private readonly object _streamLock = new();
        private GCHandle? _streamHandle;
        private CancellationTokenSource _cancellationTokenSource = new();
        public UnsafeAudioAsset UnsafeHandle { get; }

        public AudioAsset()
        {
            UnsafeHandle = UnsafeAudioAsset.Create();
        }

        internal AudioAsset(UnsafeAudioAsset handle)
        {
            UnsafeHandle = handle;
        }

        public int SampleRate => UnsafeHandle.GetSampleRate();
        public long SampleCount => UnsafeHandle.GetSampleCount();
        public int ChannelCount => UnsafeHandle.GetChannelCount();
        public long FrameCount => UnsafeHandle.GetFrameCount();

        public void Load(string path, SampleFormat sampleFormat = SampleFormat.Float)
        {
            lock (_streamLock) ReleaseStreamHandle();
            UnsafeHandle.LoadFile(path, sampleFormat);
        }

        public void Load(Span<byte> data, SampleFormat sampleFormat = SampleFormat.Float)
        {
            lock (_streamLock) ReleaseStreamHandle();
            UnsafeHandle.LoadMemory(data.ToArray(), sampleFormat);
        }

        public unsafe void LoadIO(IUniassetStream stream, SampleFormat sampleFormat = SampleFormat.Float)
        {
            if (stream == null) throw new ArgumentNullException(nameof(stream));

            lock (_streamLock)
            {
                ReleaseStreamHandle();
                var gcHandle = GCHandle.Alloc(stream);
                _streamHandle = gcHandle;

                var provider = NativeIOProvider.Default();
                provider.userData = GCHandle.ToIntPtr(gcHandle).ToPointer();

                UnsafeHandle.LoadIO(&provider, sampleFormat);
            }
        }

        private void ReleaseStreamHandle()
        {
            if (_streamHandle is { IsAllocated: true } handle)
            {
                handle.Free();
                _streamHandle = null;
            }
        }

        public long Tell()
        {
            return UnsafeHandle.Tell();
        }

        public int Read<T>(Span<T> buffer, int frameCount)
            where T : unmanaged
        {
            return UnsafeHandle.Read(buffer, frameCount);
        }

        public void Seek(long position)
        {
            UnsafeHandle.Seek(position);
        }

        public void Unload()
        {
            lock (_streamLock) ReleaseStreamHandle();
            UnsafeHandle.Unload();
        }

        public void Prepare()
        {
            UnsafeHandle.Prepare();
        }

        public Task PrepareAsync()
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Prepare();
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public AudioAsset TryClone()
        {
            var cloned = UnsafeHandle.TryClone();
            return new AudioAsset(cloned);
        }

        public AudioClip ToAudioClip(string name = "created_from_uniasset", bool stream = true)
        {
            return AudioClip.Create(name, (int)(SampleCount / ChannelCount),
                ChannelCount, SampleRate, stream, AudioClipRead, AudioClipSeek);
        }

        private void AudioClipSeek(int position)
        {
            if (Volatile.Read(ref _disposed) != 0) return;
            lock (this)
            {
                if (Volatile.Read(ref _disposed) != 0) return;
                UnsafeHandle.SeekUnsafe(position);
            }
        }

        private void AudioClipRead(float[] data)
        {
            if (Volatile.Read(ref _disposed) != 0) return;
            lock (this)
            {
                if (Volatile.Read(ref _disposed) != 0) return;
                UnsafeHandle.ReadUnsafe(new Span<float>(data), data.Length / ChannelCount);
            }
        }

        public void Dispose()
        {
            if (Interlocked.CompareExchange(ref _disposed, 1, 0) != 0) return;
            _cancellationTokenSource.Cancel();
            _cancellationTokenSource.Dispose();
            lock (_streamLock) ReleaseStreamHandle();
            UnsafeHandle.Destroy();
        }

        ~AudioAsset()
        {
            Dispose();
        }
    }
}
