

using Uniasset.Unsafe;

namespace Uniasset.Audio
{
    public sealed class AudioPlayer
    {
        public UnsafeAudioPlayer UnsafeHandle { get; } = UnsafeAudioPlayer.Create();

        public void Open(AudioAsset audioAsset)
            => UnsafeHandle.Open(audioAsset.UnsafeHandle);

        public void Close()
            => UnsafeHandle.Close();

        public void Resume()
            => UnsafeHandle.Resume();

        public void Pause()
            => UnsafeHandle.Pause();
    }
}