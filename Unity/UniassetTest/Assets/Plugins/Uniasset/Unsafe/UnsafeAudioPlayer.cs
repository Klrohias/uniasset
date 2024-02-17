using System;
using System.Runtime.InteropServices;

namespace Uniasset.Unsafe
{
    public readonly unsafe struct UnsafeAudioPlayer
    {
        public readonly void* Instance;

        public UnsafeAudioPlayer(void* instance)
        {
            Instance = instance;
        }

        public static UnsafeAudioPlayer Create()
        {
            return new UnsafeAudioPlayer(Interop.Uniasset_AudioPlayer_Create());
        }

        public void Open(UnsafeAudioAsset audioAsset)
        {
            Interop.Uniasset_AudioPlayer_Open(Instance, audioAsset.Instance);
            CheckNativeErrorInternal();
        }
        
        public void Resume()
        {
            Interop.Uniasset_AudioPlayer_Resume(Instance);
            CheckNativeErrorInternal();
        }
        
        public void Pause()
        {
            Interop.Uniasset_AudioPlayer_Pause(Instance);
            CheckNativeErrorInternal();
        }
        
        public void Close()
        {
            Interop.Uniasset_AudioPlayer_Close(Instance);
            CheckNativeErrorInternal();
        }

        public float GetTime()
        {
            var result = Interop.Uniasset_AudioPlayer_GetTime(Instance);
            CheckNativeErrorInternal();
            return result;
        }
        
        public void SetTime(float time)
        {
            Interop.Uniasset_AudioPlayer_SetTime(Instance, time);
            CheckNativeErrorInternal();
        }
        
        public float GetVolume()
        {
            var result = Interop.Uniasset_AudioPlayer_GetVolume(Instance);
            CheckNativeErrorInternal();
            return result;
        }
        
        public void SetVolume(float volume)
        {
            Interop.Uniasset_AudioPlayer_SetVolume(Instance, volume);
            CheckNativeErrorInternal();
        }

        public bool IsPaused()
        {
            var result = Interop.Uniasset_AudioPlayer_IsPaused(Instance) != 0;
            CheckNativeErrorInternal();
            return result;
        }

        public void Destroy()
        {
            Interop.Uniasset_AudioPlayer_Destory(Instance);
        }

        private void CheckNativeErrorInternal()
        {
            var errorMessage = Marshal.PtrToStringAnsi(new IntPtr(Interop.Uniasset_AudioPlayer_GetError(Instance)));

            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new NativeException(errorMessage);
        }
    }
}