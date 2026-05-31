using System;
using System.Runtime.InteropServices;
using Uniasset.Unsafe;

namespace Uniasset.Audio
{
    public abstract unsafe class ManagedAudioDecoder : AudioDecoder
    {
        protected GCHandle SelfHandle;

        private static readonly void* GetChannelCountCallbackPtr;
        private static readonly void* GetSampleCountCallbackPtr;
        private static readonly void* GetSampleFormatCallbackPtr;
        private static readonly void* GetSampleRateCallbackPtr;
        private static readonly void* ReadCallbackPtr;
        private static readonly void* SeekCallbackPtr;
        private static readonly void* TellCallbackPtr;
        private bool _disposed;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate uint GetChannelCountDelegate(void* handle);

        private static uint GetChannelCountCallback(void* handle)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            return (uint)self.ChannelCount;
        }
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate ulong GetSampleCountDelegate(void* handle);

        private static ulong GetSampleCountCallback(void* handle)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            return (ulong)self.SampleCount;
        }
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate SampleFormat GetSampleFormatDelegate(void* handle);

        private static SampleFormat GetSampleFormatCallback(void* handle)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            return self.Format;
        }
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate uint GetSampleRateDelegate(void* handle);

        private static uint GetSampleRateCallback(void* handle)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            return (uint)self.SampleRate;
        }
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate uint ReadDelegate(void* handle, void* buffer, uint frameCount);

        private static uint ReadCallback(void* handle, void* buffer, uint frameCount)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            var count = (int)frameCount;
            var frameSize = self.Format == SampleFormat.Float ? sizeof(float) : sizeof(short);
            return (uint)self.Read(new Span<byte>(buffer, count * frameSize), count);
        }
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate byte SeekDelegate(void* handle, long position);

        private static byte SeekCallback(void* handle, long position)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            return (byte)(self.Seek(position) ? 1 : 0);
        }
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate long TellDelegate(void* handle);

        private static long TellCallback(void* handle)
        {
            var gcHandle = GCHandle.FromIntPtr(new IntPtr(handle));
            var self = (ManagedAudioDecoder)gcHandle.Target;
            return self.Tell();
        }

        static ManagedAudioDecoder()
        {
            GetChannelCountCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<GetChannelCountDelegate>(GetChannelCountCallback).ToPointer();
            GetSampleCountCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<GetSampleCountDelegate>(GetSampleCountCallback).ToPointer();
            GetSampleFormatCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<GetSampleFormatDelegate>(GetSampleFormatCallback).ToPointer();
            GetSampleRateCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<GetSampleRateDelegate>(GetSampleRateCallback).ToPointer();
            ReadCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<ReadDelegate>(ReadCallback).ToPointer();
            SeekCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<SeekDelegate>(SeekCallback).ToPointer();
            TellCallbackPtr = Marshal
                .GetFunctionPointerForDelegate<TellDelegate>(TellCallback).ToPointer();
        }

        protected ManagedAudioDecoder()
        {
            SelfHandle = GCHandle.Alloc(this);

            UnsafeHandle = new UnsafeAudioDecoder(Interop.Uniasset_ExternalAudioDecoder_Create(
                GCHandle.ToIntPtr(SelfHandle).ToPointer(),
                GetChannelCountCallbackPtr,
                GetSampleCountCallbackPtr,
                GetSampleFormatCallbackPtr,
                GetSampleRateCallbackPtr,
                ReadCallbackPtr,
                SeekCallbackPtr,
                TellCallbackPtr
            ));
        }

        public override void Dispose()
        {
            if (_disposed) return;
            _disposed = true;

            UnsafeHandle.Destroy();
        }
    }
}