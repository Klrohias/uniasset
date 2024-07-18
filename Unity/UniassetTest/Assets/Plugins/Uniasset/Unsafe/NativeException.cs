using System;
using System.Runtime.InteropServices;
using Uniasset.Unsafe;

namespace Uniasset.Unsafe
{
    public class NativeException : Exception
    {
        public NativeException(string message) : base(message)
        {
        }

        public static unsafe void ThrowIfNeeded()
        {
            if (Interop.Uniasset_HasError() == 0)
            {
                return;
            }
            
            var errorMessage = Marshal.PtrToStringAnsi(new IntPtr(Interop.Uniasset_GetError()));

            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new NativeException(errorMessage);
        }
    }
}