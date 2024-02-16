using System;
using Uniasset.Unsafe;

namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable
    {
        private bool _disposed = false;
        public UnsafeAudioAsset UnsafeHandle { get; } = UnsafeAudioAsset.Create();

        public int SampleRate => UnsafeHandle.GetSampleRate();
        public long SampleCount => UnsafeHandle.GetSampleCountLong();
        public int ChannelCount => UnsafeHandle.GetChannelCount();
        public float Length => UnsafeHandle.GetLength();

        public void Load(string path)
        {
            UnsafeHandle.Load(path);
        }

        public void Load(Span<byte> data)
        {
            UnsafeHandle.Load(data.ToArray());
        }

        public void Unload()
        {
            UnsafeHandle.Unload();
        }
        
        public NativeAudioDecoder GetAudioDecoder(SampleFormat format = SampleFormat.Float)
        {
            return new NativeAudioDecoder(UnsafeHandle.GetAudioDecoder(format));
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