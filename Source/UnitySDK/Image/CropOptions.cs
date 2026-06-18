using System.Runtime.InteropServices;

namespace Uniasset.Image
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CropOptions
    {
        public uint X;
        public uint Y;
        public uint Width;
        public uint Height;

        public CropOptions(int x, int y, int width, int height)
        {
            X = checked((uint)x);
            Y = checked((uint)y);
            Width = checked((uint)width);
            Height = checked((uint)height);
        }
    }
}
