using System;

namespace Uniasset
{
    public interface IUniassetStream
    {
        int Read(Span<byte> buffer);

        long Seek(long offset, SeekOrigin origin);
    }

    public enum SeekOrigin
    {
        Begin = 0,
        Current = 1,
        End = 2,
    }
}

