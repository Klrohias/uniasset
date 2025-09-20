using System;

namespace Uniasset.Unsafe
{
    public unsafe struct UnsafePlaybackInstance
    {
        public readonly void* Instance;
        
        public UnsafePlaybackInstance(void* instance)
        {
            Instance = instance;
        }

        public void Destroy()
        {
            Interop.Uniasset_PlaybackInstance_Destory(Instance);
        }

        public float GetVolume()
        {
            var volume = Interop.Uniasset_PlaybackInstance_GetVolume(Instance);
            NativeException.ThrowIfNeeded();
            return volume;
        }
        
        public void SetVolume(float volume)
        {
            Interop.Uniasset_PlaybackInstance_SetVolume(Instance, volume);
            NativeException.ThrowIfNeeded();
        }

        public float GetTime()
        {
            var time = Interop.Uniasset_PlaybackInstance_GetTime(Instance);
            NativeException.ThrowIfNeeded();
            return time;
        }
        
        public void SetTime(float time)
        {
            Interop.Uniasset_PlaybackInstance_SetTime(Instance, time);
            NativeException.ThrowIfNeeded();
        }

        public ulong GetFrameTime()
        {
            var frame = Interop.Uniasset_PlaybackInstance_GetFrameTime(Instance);
            NativeException.ThrowIfNeeded();
            return frame;
        }

        public void SetFrameTime(ulong frame)
        {
            Interop.Uniasset_PlaybackInstance_SetFrameTime(Instance, frame);
            NativeException.ThrowIfNeeded();
        }
        
        public void Play()
        {
            Interop.Uniasset_PlaybackInstance_Play(Instance);
            NativeException.ThrowIfNeeded();
        }
        
        public void Stop()
        {
            Interop.Uniasset_PlaybackInstance_Stop(Instance);
            NativeException.ThrowIfNeeded();
        }
        
        public bool IsPlaying()
        {
            var isPlaying = Interop.Uniasset_PlaybackInstance_IsPlaying(Instance);
            NativeException.ThrowIfNeeded();
            return Convert.ToBoolean(isPlaying);
        }

        public void PlayScheduled(ulong frame)
        {
            Interop.Uniasset_PlaybackInstance_PlayScheduled(Instance, frame);
            NativeException.ThrowIfNeeded();
        }
        
        public void StopScheduled(ulong frame)
        {
            Interop.Uniasset_PlaybackInstance_StopScheduled(Instance, frame);
            NativeException.ThrowIfNeeded();
        }

        public bool IsLooping()
        {
            var isLooping = Interop.Uniasset_PlaybackInstance_IsLooping(Instance);
            NativeException.ThrowIfNeeded();
            return Convert.ToBoolean(isLooping);
        }
        
        public void SetLooping(bool looping)
        {
            Interop.Uniasset_PlaybackInstance_SetLooping(Instance, Convert.ToByte(looping));
            NativeException.ThrowIfNeeded();
        }
    }
}