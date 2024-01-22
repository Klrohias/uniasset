using System;
using System.Runtime.InteropServices;

namespace Uniasset
{
    public class AudioAsset : IDisposable
    {
        public IntPtr Handle { get; } = Interop.Uniasset_AudioAsset_Create();
        public int ChannelCount
        {
            get
            {
                var result = (int)Interop.Uniasset_AudioAsset_GetChannelCount(Handle);
                ThrowException();
                return result;
            }
        }

        public int SampleRate
        {
            get
            {
                var result = (int)Interop.Uniasset_AudioAsset_GetSampleRate(Handle);
                ThrowException();
                return result;
            }
        }

        public long SampleCount
        {
            get
            {
                var result = (long)Interop.Uniasset_AudioAsset_GetSampleCount(Handle);
                ThrowException();
                return result;
            }
        }
        
        public float Length
        {
            get
            {
                var result = Interop.Uniasset_AudioAsset_GetLength(Handle);
                ThrowException();
                return result;
            }
        }

        public AudioAsset()
        {
            if (Handle == IntPtr.Zero)
            {
                throw new Exception("Failed to create AudioAsset instance");
            }
        }
        
        public void Dispose()
        {
            Interop.Uniasset_AudioAsset_Free(Handle);
        }

        public void Load(string path)
        {
            var pathPtr = Marshal.StringToHGlobalAnsi(path);
            try
            {
                Interop.Uniasset_AudioAsset_LoadFile(Handle, pathPtr);
                ThrowException();
            }
            finally
            {
                Marshal.FreeHGlobal(pathPtr);
            }
        }

        public unsafe void Load(Span<byte> data)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_AudioAsset_Load(Handle, new IntPtr(imageData), Convert.ToUInt64(data.Length));
                ThrowException();
            }
        }

        public void Unload()
        {
            Interop.Uniasset_AudioAsset_Unload(Handle);
        }

        private void ThrowException()
        {
            var errorMessage = Marshal.PtrToStringAuto(Interop.Uniasset_AudioAsset_GetError(Handle));
            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new UniassetNativeException(errorMessage);
        }
    }
}