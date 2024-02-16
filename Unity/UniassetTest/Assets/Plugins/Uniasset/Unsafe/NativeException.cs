using System;

namespace Uniasset
{
    public class NativeException : Exception
    {
        public NativeException(string message) : base(message)
        {
        }
    }
}