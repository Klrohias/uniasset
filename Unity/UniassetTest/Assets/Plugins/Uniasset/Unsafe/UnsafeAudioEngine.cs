using System;

namespace Uniasset.Unsafe
{
    public unsafe struct UnsafeAudioEngine
    {
        public readonly void* Instance;
        
        public UnsafeAudioEngine(void* instance)
        {
            Instance = instance;
        }
        
        public static UnsafeAudioEngine Create()
        {
            return new UnsafeAudioEngine(Interop.Uniasset_AudioEngine_Create());
        }
        
        public void Destroy()
        {
            Interop.Uniasset_AudioEngine_Destory(Instance);
        }

        public float GetVolume()
        {
            var volume = Interop.Uniasset_AudioEngine_GetVolume(Instance);
            NativeException.ThrowIfNeeded();
            return volume;
        }
        
        public void SetVolume(float volume)
        {
            Interop.Uniasset_AudioEngine_SetVolume(Instance, volume);
            NativeException.ThrowIfNeeded();
        }

        public ulong GetTimeInPcmFrames()
        {
            var time = Interop.Uniasset_AudioEngine_GetTimeInPcmFrames(Instance);
            NativeException.ThrowIfNeeded();
            return time;
        }

        public void ResetTimeInPcmFrames()
        {
            Interop.Uniasset_AudioEngine_ResetTimeInPcmFrames(Instance);
            NativeException.ThrowIfNeeded();
        }

        public uint GetSampleRate()
        {
            var rate = Interop.Uniasset_AudioEngine_GetSampleRate(Instance);
            NativeException.ThrowIfNeeded();
            return rate;
        }
        
        public UnsafePlaybackInstance CreatePlaybackInstance(UnsafeAudioDecoder decoder)
        {
            var instance = Interop.Uniasset_AudioEngine_CreatePlayback(Instance, decoder.Instance);
            NativeException.ThrowIfNeeded();
            return new UnsafePlaybackInstance(instance);
        }
    }
}