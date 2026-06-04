using System;
using System.Runtime.InteropServices;
using Uniasset.Unsafe;
using UnityEngine;

namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable
    {
        private bool _disposed = false;
        public UnsafeAudioAsset UnsafeHandle { get; } = UnsafeAudioAsset.Create();

        public int SampleRate => UnsafeHandle.GetSampleRate();
        public long SampleCount => UnsafeHandle.GetSampleCount();
        public int ChannelCount => UnsafeHandle.GetChannelCount();
        public long FrameCount => UnsafeHandle.GetFrameCount();

        public void Load(string path, SampleFormat sampleFormat = SampleFormat.Int16)
        {
            UnsafeHandle.LoadFile(path, sampleFormat);
        }

        public void Load(Span<byte> data, SampleFormat sampleFormat = SampleFormat.Int16)
        {
            UnsafeHandle.LoadMemory(data.ToArray(), sampleFormat);
        }

        public unsafe void LoadIO(IUniassetStream stream, SampleFormat sampleFormat = SampleFormat.Int16)
        {
            if (stream == null) throw new ArgumentNullException(nameof(stream));

            var gcHandle = GCHandle.Alloc(stream);
            try
            {
                var provider = Interop.NativeIOProvider.Default();
                provider.userData = GCHandle.ToIntPtr(gcHandle).ToPointer();

                UnsafeHandle.LoadIO(&provider, sampleFormat);
            }
            finally
            {
                gcHandle.Free();
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

        public AudioClip ToAudioClip(string name = "created_from_uniasset", bool stream = true)
        {
            return AudioClip.Create(name, (int)(SampleCount / ChannelCount),
                ChannelCount, SampleRate, stream, AudioClipRead, AudioClipSeek);
        }

        private void AudioClipSeek(int position)
        {
            lock (this)
            {
                Seek(position);
            }
        }

        private void AudioClipRead(float[] data)
        {
            lock (this)
            {
                Read(new Span<float>(data), data.Length / ChannelCount);
            }
        }

        public void Dispose()
        {
            if (_disposed) return;
            _disposed = true;
            UnsafeHandle.Destroy();
        }

        ~AudioAsset()
        {
            Dispose();
        }
    }
}
