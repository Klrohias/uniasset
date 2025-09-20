using System;
using System.Buffers;
using System.Runtime.InteropServices;

using Uniasset.Unsafe;

namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable
    {
        private bool _disposed = false;
        private string _path = null;
        
        public UnsafeAudioAsset UnsafeHandle { get; } = UnsafeAudioAsset.Create();

        public int SampleRate => UnsafeHandle.GetSampleRate();
        public long SampleCount => UnsafeHandle.GetSampleCountLong();
        public int ChannelCount => UnsafeHandle.GetChannelCount();
        public float Length => UnsafeHandle.GetLength();

        public string Path
        {
            get
            {
                return _path ??= UnsafeHandle.GetPath();
            }
        }

        public void Load(string path)
        {
            UnsafeHandle.Load(path);
        }

        public void Load(Span<byte> data)
        {
            UnsafeHandle.Load(data.ToArray());
        }

        public void LoadAdopt(NativeMemoryManager manager)
        {
            if (manager == null)
                throw new ArgumentNullException(nameof(manager));
            
            if (!manager.Release(out var addr, out var length))
                throw new ObjectDisposedException("NativeMemoryManager is invalid");
            
            UnsafeHandle.LoadAdopt(addr, (ulong)length);
        }

        public void LoadAdoptUnsafe(uint addr, int size)
        {
            UnsafeHandle.LoadAdopt((nint)addr, (ulong)size);
        }

        public void Unload()
        {
            UnsafeHandle.Unload();
            _path = null;
        }
        
        public NativeAudioDecoder GetAudioDecoder(SampleFormat format = SampleFormat.Float, long frameBufferSize = -1)
        {
            return new NativeAudioDecoder(UnsafeHandle.GetAudioDecoder(format, frameBufferSize));
        }

        public void Dispose()
        {
            if (_disposed) return;
            _disposed = true;
            UnsafeHandle.Destroy();
        }

        ~AudioAsset()
        {
            Dispose();
        }
    }
}