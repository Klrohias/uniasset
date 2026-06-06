using System;
using System.Runtime.InteropServices;
using Uniasset;

namespace Uniasset.Unsafe
{
    public static unsafe partial class Interop
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate long NativeReadFn(void* userData, byte* buf, nuint count);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate long NativeSeekFn(void* userData, long offset, int whence);

        [StructLayout(LayoutKind.Sequential)]
        public struct NativeIOProvider
        {
            public void* userData;
            public void* readCb;
            public void* seekCb;

            private static readonly NativeReadFn ReadCallbackDelegate = ReadCallback;
            private static readonly NativeSeekFn SeekCallbackDelegate = SeekCallback;
            private static readonly void* ReadCallbackPtr =
                Marshal.GetFunctionPointerForDelegate(ReadCallbackDelegate).ToPointer();
            private static readonly void* SeekCallbackPtr =
                Marshal.GetFunctionPointerForDelegate(SeekCallbackDelegate).ToPointer();

            [AOT.MonoPInvokeCallback(typeof(NativeReadFn))]
            private static long ReadCallback(void* userData, byte* buf, nuint count)
            {
                try
                {
                    var handle = GCHandle.FromIntPtr(new IntPtr(userData));
                    var stream = (IUniassetStream)handle.Target;
                    if (stream == null) return -1;

                    var span = new Span<byte>(buf, checked((int)count));
                    var read = stream.Read(span);
                    return read < 0 ? -1 : read;
                }
                catch
                {
                    return -1;
                }
            }

            [AOT.MonoPInvokeCallback(typeof(NativeSeekFn))]
            private static long SeekCallback(void* userData, long offset, int whence)
            {
                try
                {
                    var handle = GCHandle.FromIntPtr(new IntPtr(userData));
                    var stream = (IUniassetStream)handle.Target;
                    if (stream == null) return -1;

                    var origin = whence switch
                    {
                        0 => SeekOrigin.Begin,
                        1 => SeekOrigin.Current,
                        2 => SeekOrigin.End,
                        _ => throw new ArgumentOutOfRangeException(nameof(whence), whence, null),
                    };

                    return stream.Seek(offset, origin);
                }
                catch
                {
                    return -1;
                }
            }

            public static NativeIOProvider Default()
            {
                return new NativeIOProvider
                {
                    userData = null,
                    readCb = ReadCallbackPtr,
                    seekCb = SeekCallbackPtr,
                };
            }
        }
    }
}
