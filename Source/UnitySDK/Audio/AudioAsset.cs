using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Uniasset.Unsafe;
using UnityEngine;

namespace Uniasset.Audio
{
    public sealed class AudioAsset : IDisposable
    {
        /// <summary>
        /// Merged state: -1 = disposed, 0 = idle, &gt;0 = active unsafe callbacks.
        /// </summary>
        private int _inUse;
        private readonly object _lock = new();
        private GCHandle? _streamHandle;
        private IntPtr _ioProviderPtr;
        private CancellationTokenSource _cancellationTokenSource = new();
        public UnsafeAudioAsset UnsafeHandle { get; }

        public AudioAsset()
        {
            UnsafeHandle = UnsafeAudioAsset.Create();
        }

        internal AudioAsset(UnsafeAudioAsset handle)
        {
            UnsafeHandle = handle;
        }

        public int SampleRate { get { ThrowIfDisposed(); return UnsafeHandle.GetSampleRate(); } }
        public long SampleCount { get { ThrowIfDisposed(); return UnsafeHandle.GetSampleCount(); } }
        public int ChannelCount { get { ThrowIfDisposed(); return UnsafeHandle.GetChannelCount(); } }
        public long FrameCount { get { ThrowIfDisposed(); return UnsafeHandle.GetFrameCount(); } }
        public SampleFormat SampleFormat { get { ThrowIfDisposed(); return UnsafeHandle.GetSampleFormat(); } }

        public void Load(string path, SampleFormat sampleFormat = SampleFormat.Float)
        {
            ThrowIfDisposed();
            UnsafeHandle.LoadFile(path, sampleFormat);
            lock (_lock) ReleaseStreamHandle();
        }

        public void Load(Span<byte> data, SampleFormat sampleFormat = SampleFormat.Float)
        {
            ThrowIfDisposed();
            UnsafeHandle.LoadMemory(data.ToArray(), sampleFormat);
            lock (_lock) ReleaseStreamHandle();
        }

        public unsafe void LoadIO(IUniassetStream stream, SampleFormat sampleFormat = SampleFormat.Float)
        {
            ThrowIfDisposed();
            if (stream == null) throw new ArgumentNullException(nameof(stream));

            lock (_lock)
            {
                ReleaseStreamHandle();

                var gcHandle = GCHandle.Alloc(stream);
                _streamHandle = gcHandle;

                // Allocate on unmanaged heap — native caches the provider pointer
                // for streaming callbacks beyond the LoadIO call.
                _ioProviderPtr = Marshal.AllocHGlobal(sizeof(NativeIOProvider));
                var provider = NativeIOProvider.Default();
                provider.userData = GCHandle.ToIntPtr(gcHandle).ToPointer();
                Marshal.StructureToPtr(provider, _ioProviderPtr, false);

                UnsafeHandle.LoadIO((NativeIOProvider*)_ioProviderPtr, sampleFormat);
            }
        }

        private void ReleaseStreamHandle()
        {
            if (_streamHandle is { IsAllocated: true } handle)
            {
                handle.Free();
                _streamHandle = null;
            }

            if (_ioProviderPtr != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(_ioProviderPtr);
                _ioProviderPtr = IntPtr.Zero;
            }
        }

        private void ThrowIfDisposed()
        {
            if (Volatile.Read(ref _inUse) == -1)
                throw new ObjectDisposedException(nameof(AudioAsset));
        }

        public long Tell()
        {
            ThrowIfDisposed();
            return UnsafeHandle.Tell();
        }

        public int Read<T>(Span<T> buffer, int frameCount)
            where T : unmanaged
        {
            ThrowIfDisposed();
            return UnsafeHandle.Read(buffer, frameCount);
        }

        public void Seek(long position)
        {
            ThrowIfDisposed();
            UnsafeHandle.Seek(position);
        }

        public void Unload()
        {
            ThrowIfDisposed();
            UnsafeHandle.Unload();
            lock (_lock) ReleaseStreamHandle();
        }

        public void Prepare()
        {
            ThrowIfDisposed();
            UnsafeHandle.Prepare();
        }

        public Task PrepareAsync()
        {
            ThrowIfDisposed();
            return Task.Factory.StartNew(() =>
            {
                Prepare();
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public AudioAsset TryClone()
        {
            ThrowIfDisposed();
            var cloned = UnsafeHandle.TryClone();
            return new AudioAsset(cloned);
        }

        public AudioClip ToAudioClip(string name = "created_from_uniasset", bool stream = true)
        {
            ThrowIfDisposed();
            return AudioClip.Create(name, (int)(SampleCount / ChannelCount),
                ChannelCount, SampleRate, stream, AudioClipRead, AudioClipSeek);
        }

        private void AudioClipSeek(int position)
        {
            int oldState, newState;
            do
            {
                oldState = Volatile.Read(ref _inUse);
                if (oldState == -1) return;
                newState = oldState + 1;
            } while (Interlocked.CompareExchange(ref _inUse, newState, oldState) != oldState);

            try
            {
                UnsafeHandle.SeekUnsafe(position);
            }
            finally
            {
                Interlocked.Decrement(ref _inUse);
            }
        }

        private void AudioClipRead(float[] data)
        {
            int oldState, newState;
            do
            {
                oldState = Volatile.Read(ref _inUse);
                if (oldState == -1) return;
                newState = oldState + 1;
            } while (Interlocked.CompareExchange(ref _inUse, newState, oldState) != oldState);

            try
            {
                UnsafeHandle.ReadUnsafe(new Span<float>(data), data.Length / ChannelCount);
            }
            finally
            {
                Interlocked.Decrement(ref _inUse);
            }
        }

        public void Dispose()
        {
            // Claim the disposed state: spin until _inUse == 0, then CAS to -1.
            var spinWait = new SpinWait();
            while (true)
            {
                var current = Volatile.Read(ref _inUse);
                if (current == -1) return;
                if (current == 0)
                {
                    if (Interlocked.CompareExchange(ref _inUse, -1, 0) == 0)
                        break;
                }
                spinWait.SpinOnce();
            }

            _cancellationTokenSource.Cancel();
            _cancellationTokenSource.Dispose();

            lock (_lock)
            {
                UnsafeHandle.Destroy();
                ReleaseStreamHandle();
            }
        }

        ~AudioAsset()
        {
            Dispose();
        }
    }
}
