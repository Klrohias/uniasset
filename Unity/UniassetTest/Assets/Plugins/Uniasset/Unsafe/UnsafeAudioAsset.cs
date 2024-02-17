using System;
using System.Runtime.InteropServices;
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

        public void Load(byte[] data)
        {
            fixed (byte* dataPtr = data)
            {
                Interop.Uniasset_AudioAsset_Load(Instance, dataPtr, (ulong)data.Length);
                CheckNativeErrorInternal();
            }
        }

        public void Load(string path)
        {
            var pathBytes = Encoding.Default.GetBytes(path);
            fixed (byte* pathPtr = pathBytes)
            {
                Interop.Uniasset_AudioAsset_LoadFile(Instance, (sbyte*)pathPtr);
                CheckNativeErrorInternal();
            }
        }

        public void Unload()
        {
            Interop.Uniasset_AudioAsset_Unload(Instance);
            CheckNativeErrorInternal();
        }

        public int GetSampleRate()
        {
            var result = (int)Interop.Uniasset_AudioAsset_GetSampleRate(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public int GetSampleCount()
        {
            var result = (int)Interop.Uniasset_AudioAsset_GetSampleCount(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public long GetSampleCountLong()
        {
            var result = (long)Interop.Uniasset_AudioAsset_GetSampleCount(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public int GetChannelCount()
        {
            var result = (int)Interop.Uniasset_AudioAsset_GetChannelCount(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public float GetLength()
        {
            var result = Interop.Uniasset_AudioAsset_GetLength(Instance);
            CheckNativeErrorInternal();
            return result;
        }
        
        public UnsafeAudioDecoder GetAudioDecoder(SampleFormat format, long frameBufferSize)
        {
            var result = Interop.Uniasset_AudioAsset_GetAudioDecoder(Instance, (byte)format, frameBufferSize);
            CheckNativeErrorInternal();
            return new UnsafeAudioDecoder(result);
        }


        public void Destroy()
        {
            Interop.Uniasset_AudioAsset_Destory(Instance);
        }
        
        private void CheckNativeErrorInternal()
        {
            var errorMessage = Marshal.PtrToStringAnsi(new IntPtr(Interop.Uniasset_AudioAsset_GetError(Instance)));

            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new NativeException(errorMessage);
        }
    }
}