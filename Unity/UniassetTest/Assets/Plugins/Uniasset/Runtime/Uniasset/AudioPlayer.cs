using System;
using System.Runtime.InteropServices;

namespace Uniasset
{
    public class AudioPlayer : IDisposable
    {
        private readonly IntPtr _handle = Interop.Uniasset_AudioPlayer_Create();

        public bool IsPaused => Interop.Uniasset_AudioPlayer_IsPaused(_handle) != 0;

        public float Volume
        {
            get => Interop.Uniasset_AudioPlayer_GetVolume(_handle);
            set => Interop.Uniasset_AudioPlayer_SetVolume(_handle, value);
        }

        public void Dispose()
        {
            Interop.Uniasset_AudioPlayer_Free(_handle);
        }

        private void ThrowException()
        {
            var errorMessage = Marshal.PtrToStringAuto(Interop.Uniasset_AudioPlayer_GetError(_handle));
            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new UniassetNativeException(errorMessage);
        }

        public void Open(AudioAsset audioAsset)
        {
            if (Interop.Uniasset_AudioPlayer_Open(_handle, audioAsset.Handle) == 0)
                ThrowException();
        }

        public void Resume()
        {
            Interop.Uniasset_AudioPlayer_Resume(_handle);
        }

        public void Pause()
        {
            Interop.Uniasset_AudioPlayer_Pause(_handle);
        }

        public void Close()
        {
            Interop.Uniasset_AudioPlayer_Close(_handle);
        }
    }
}