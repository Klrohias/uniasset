using System;
using System.IO;

namespace Uniasset
{
    public sealed class StreamWrapper : IUniassetStream
    {
        private readonly Stream _stream;

        public StreamWrapper(Stream stream)
        {
            _stream = stream ?? throw new ArgumentNullException(nameof(stream));
            if (!stream.CanRead) throw new ArgumentException("Stream must be readable.", nameof(stream));
            if (!stream.CanSeek) throw new ArgumentException("Stream must be seekable.", nameof(stream));
        }

        public int Read(Span<byte> buffer) => _stream.Read(buffer);

        public long Seek(long offset, SeekOrigin origin)
        {
            var dotNetOrigin = origin switch
            {
                SeekOrigin.Begin => System.IO.SeekOrigin.Begin,
                SeekOrigin.Current => System.IO.SeekOrigin.Current,
                SeekOrigin.End => System.IO.SeekOrigin.End,
                _ => throw new ArgumentOutOfRangeException(nameof(origin), origin, null),
            };

            return _stream.Seek(offset, dotNetOrigin);
        }
    }
}

