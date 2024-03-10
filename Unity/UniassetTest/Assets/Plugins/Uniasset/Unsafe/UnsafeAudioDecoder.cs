using System;
using Uniasset.Audio;

namespace Uniasset.Unsafe
{
    public readonly unsafe struct UnsafeAudioDecoder
    {
        public readonly void* Instance;

        public UnsafeAudioDecoder(void* instance)
        {
            Instance = instance;
        }

        public int GetSampleRate()
        {
            return (int)Interop.Uniasset_IAudioDecoder_GetSampleRate(Instance);
        }

        public int GetSampleCount()
        {
            return (int)Interop.Uniasset_IAudioDecoder_GetSampleCount(Instance);
        }

        public long GetSampleCountLong()
        {
            return (long)Interop.Uniasset_IAudioDecoder_GetSampleCount(Instance);
        }

        public int GetChannelCount()
        {
            return (int)Interop.Uniasset_IAudioDecoder_GetChannelCount(Instance);
        }

        public SampleFormat GetSampleFormat()
        {
            return (SampleFormat)Interop.Uniasset_IAudioDecoder_GetSampleFormat(Instance);
        }
        
        public long Tell()
        {
            return Interop.Uniasset_IAudioDecoder_Tell(Instance);
        }

        public bool Seek(long pos)
        {
            return Interop.Uniasset_IAudioDecoder_Seek(Instance, pos) != 0;
        }

        public int Read<T>(Span<T> buffer, int frameCount)
            where T : unmanaged
        {
            fixed (T* bufferPtr = buffer)
            {
                return (int)Interop.Uniasset_IAudioDecoder_Read(Instance, bufferPtr, (uint)frameCount);
            }
        }

        public void Destroy()
        {
            Interop.Uniasset_IAudioDecoder_Destory(Instance);
        }
    }
}