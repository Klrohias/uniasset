using System;
using Uniasset.Unsafe;
using UnityEngine;

namespace Uniasset.Audio
{
    public abstract class AudioDecoder : IDisposable
    {
        public UnsafeAudioDecoder UnsafeHandle { get; protected set; }

        public abstract int SampleRate { get; }

        public abstract long SampleCount { get; }

        public abstract int ChannelCount { get; }
        public abstract SampleFormat Format { get; }

        public abstract int Read<T>(Span<T> buffer, int frameCount)
            where T : unmanaged;

        public abstract bool Seek(long samplePosition);
        public abstract long Tell();
        public abstract void Dispose();

        public AudioClip ToAudioClip(string name = "created_from_uniasset", bool stream = false)
        {
            if (Format != SampleFormat.Float)
                Debug.LogWarning("Uniasset: If you want to create a audio clip from AudioDecoder, " +
                                 "you should use Float format or it will cause unexpected problem.");
            return AudioClip.Create(name, (int)SampleCount / ChannelCount
                , ChannelCount, SampleRate, stream, AudioClipRead, AudioClipSeek);
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
    }
}