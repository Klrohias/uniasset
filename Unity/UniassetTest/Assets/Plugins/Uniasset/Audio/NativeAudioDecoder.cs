using System;
using Uniasset.Unsafe;

namespace Uniasset.Audio
{
    public sealed class NativeAudioDecoder : AudioDecoder
    {
        private bool _disposed;

        public NativeAudioDecoder(UnsafeAudioDecoder audioDecoder)
        {
            UnsafeHandle = audioDecoder;
            SampleRate = UnsafeHandle.GetSampleRate();
            SampleCount = UnsafeHandle.GetSampleCountLong();
            ChannelCount = UnsafeHandle.GetChannelCount();
            Format = UnsafeHandle.GetSampleFormat();
        }

        public override int SampleRate { get; }
        public override long SampleCount { get; }
        public override int ChannelCount { get; }
        public override SampleFormat Format { get; }

        public override int Read<T>(Span<T> buffer, int frameCount)
        {
            return UnsafeHandle.Read(buffer, frameCount);
        }

        public override bool Seek(long samplePosition)
        {
            return UnsafeHandle.Seek(samplePosition);
        }

        public override long Tell()
        {
            return UnsafeHandle.Tell();
        }

        public override void Dispose()
        {
            if (_disposed) return;
            _disposed = true;

            UnsafeHandle.Destroy();
        }

        ~NativeAudioDecoder()
        {
            Dispose();
        }
    }
}