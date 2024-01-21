using System;
using System.Runtime.InteropServices;

namespace Uniasset
{
    public class AudioAsset : IDisposable
    {
        private readonly IntPtr _handle = Interop.Uniasset_AudioAsset_Create();
        internal IntPtr Handle => _handle;

        public AudioAsset()
        {
            if (_handle == IntPtr.Zero)
            {
                throw new Exception("Failed to create AudioAsset instance");
            }
        }
        
        public void Dispose()
        {
            Interop.Uniasset_AudioAsset_Free(_handle);
        }

        public void Load(string path)
        {
            var pathPtr = Marshal.StringToHGlobalAnsi(path);
            try
            {
                if (Interop.Uniasset_AudioAsset_LoadFile(_handle, pathPtr) == 0)
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
                if (Interop.Uniasset_AudioAsset_Load(_handle,
                        new IntPtr(imageData), Convert.ToUInt64(data.Length)) == 0)
                    ThrowException();
            }
        }

        public void Unload()
        {
            Interop.Uniasset_AudioAsset_Unload(_handle);
        }
        
        private void ThrowException()
        {
            var errorMessage = Marshal.PtrToStringAuto(Interop.Uniasset_AudioAsset_GetError(_handle));
            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new UniassetNativeException(errorMessage);
        }
    }
}