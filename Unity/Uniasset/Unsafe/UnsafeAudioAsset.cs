using System;
using System.Text;
using Uniasset.Audio;

namespace Uniasset.Unsafe
{
    public readonly unsafe struct UnsafeAudioAsset
    {
        public readonly void* Instance;

        public UnsafeAudioAsset(void* instance)
        {
            Instance = instance;
        }

        public static UnsafeAudioAsset Create()
        {
            return new UnsafeAudioAsset(Interop.Uniasset_AudioAsset_Create());
        }

        public void LoadFile(string path, SampleFormat sampleFormat = SampleFormat.Int16)
        {
            var pathBytes = Encoding.UTF8.GetBytes(path);
            fixed (byte* pathPtr = pathBytes)
            {
                Interop.Uniasset_AudioAsset_LoadFile(Instance, (sbyte*)pathPtr, (byte)sampleFormat);
                NativeException.ThrowIfNeeded();
            }
        }

        public void LoadMemory(byte[] data, SampleFormat sampleFormat = SampleFormat.Int16)
        {
            fixed (byte* dataPtr = data)
            {
                Interop.Uniasset_AudioAsset_LoadMemory(Instance, dataPtr, (ulong)data.Length, (byte)sampleFormat);
                NativeException.ThrowIfNeeded();
            }
        }

        public void LoadIO(Interop.NativeIOProvider* provider, SampleFormat sampleFormat = SampleFormat.Int16)
        {
            Interop.Uniasset_AudioAsset_LoadIO(Instance, provider, (byte)sampleFormat);
            NativeException.ThrowIfNeeded();
        }

        public int GetSampleRate()
        {
            var result = (int)Interop.Uniasset_AudioAsset_GetSampleRate(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public long GetSampleCount()
        {
            var result = (long)Interop.Uniasset_AudioAsset_GetSampleCount(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public int GetChannelCount()
        {
            var result = (int)Interop.Uniasset_AudioAsset_GetChannelCount(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public long GetFrameCount()
        {
            var result = (long)Interop.Uniasset_AudioAsset_GetFrameCount(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public long Tell()
        {
            var result = Interop.Uniasset_AudioAsset_Tell(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public int Read(byte[] buffer, int frameCount)
        {
            fixed (byte* bufferPtr = buffer)
            {
                var result = (int)Interop.Uniasset_AudioAsset_Read(Instance, bufferPtr, (ulong)buffer.Length, (uint)frameCount);
                NativeException.ThrowIfNeeded();
                return result;
            }
        }

        public int Read<T>(Span<T> buffer, int frameCount)
            where T : unmanaged
        {
            fixed (T* bufferPtr = buffer)
            {
                var byteSize = (ulong)(buffer.Length * sizeof(T));
                var result = (int)Interop.Uniasset_AudioAsset_Read(Instance, (byte*)bufferPtr, byteSize, (uint)frameCount);
                NativeException.ThrowIfNeeded();
                return result;
            }
        }

        public void Seek(long position)
        {
            Interop.Uniasset_AudioAsset_Seek(Instance, position);
            NativeException.ThrowIfNeeded();
        }

        public void Destroy()
        {
            Interop.Uniasset_AudioAsset_Destory(Instance);
        }
    }
}
