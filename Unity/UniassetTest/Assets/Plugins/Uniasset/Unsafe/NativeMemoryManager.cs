using System;
using System.Buffers;
using System.Runtime.InteropServices;
using System.Threading;

namespace Uniasset.Unsafe
{
    public class NativeMemoryManager : MemoryManager<byte>
    {
        private IntPtr _addr;
        private int _length;
        
        private NativeMemoryManager(nint addr, int length)
        {
            _addr = addr;
            _length = length;
        }
        
        protected override void Dispose(bool disposing)
        {
            if (_addr == IntPtr.Zero) return;
            Marshal.FreeHGlobal(_addr);
        }

        public override unsafe Span<byte> GetSpan()
        {
            if (_addr == IntPtr.Zero)
                throw new ObjectDisposedException("NativeMemoryManager have been disposed or released");
            
            return new Span<byte>((void*)_addr, _length);
        }

        public override MemoryHandle Pin(int elementIndex = 0)
        {
            throw new NotSupportedException("Pin is not supported. Use Release to get the pointer and length.");
        }

        public override void Unpin()
        {
            throw new NotSupportedException("Unpin is not supported. Use Release to get the pointer and length.");
        }

        public bool Release(out IntPtr addr, out int length)
        {
            addr = Interlocked.Exchange(ref _addr, IntPtr.Zero);
            if (addr == IntPtr.Zero)
            {
                addr = IntPtr.Zero;
                length = 0;
                return false;
            }
            
            length = _length;
            _addr = IntPtr.Zero;
            _length = 0;
            return true;
        }
        
        public static NativeMemoryManager Allocate(int size)
        {
            if (size <= 0)
                throw new ArgumentOutOfRangeException(nameof(size), "Size must be greater than zero.");
            
            var addr = Marshal.AllocHGlobal(size);
            if (addr == (nint)0)
                throw new OutOfMemoryException("Failed to allocate native memory.");
            
            return new NativeMemoryManager(addr, size);
        }
    }
}