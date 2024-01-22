using System;
using System.Runtime.InteropServices;

namespace Uniasset
{
    public class AudioPlayer : IDisposable
    {
        public IntPtr Handle { get; } = Interop.Uniasset_AudioPlayer_Create();

        public bool IsPaused => Interop.Uniasset_AudioPlayer_IsPaused(Handle) != 0;

        public float Volume
        {
            get => Interop.Uniasset_AudioPlayer_GetVolume(Handle);
            set => Interop.Uniasset_AudioPlayer_SetVolume(Handle, value);
        }

        public float Time => Interop.Uniasset_AudioPlayer_GetTime(Handle);

        public void Dispose()
        {
            Interop.Uniasset_AudioPlayer_Free(Handle);
        }

        private void ThrowException()
        {
            var errorMessage = Marshal.PtrToStringAuto(Interop.Uniasset_AudioPlayer_GetError(Handle));
            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new UniassetNativeException(errorMessage);
        }

        public void Open(AudioAsset audioAsset)
        {
            Interop.Uniasset_AudioPlayer_Open(Handle, audioAsset.Handle);
            ThrowException();
        }

        public void Resume()
        {
            Interop.Uniasset_AudioPlayer_Resume(Handle);
        }

        public void Pause()
        {
            Interop.Uniasset_AudioPlayer_Pause(Handle);
        }

        public void Close()
        {
            Interop.Uniasset_AudioPlayer_Close(Handle);
        }
    }
}