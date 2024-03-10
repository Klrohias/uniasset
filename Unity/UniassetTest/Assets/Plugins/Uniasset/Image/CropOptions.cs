using System.Runtime.InteropServices;

namespace Uniasset.Image
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CropOptions
    {
        public int X;
        public int Y;
        public int Width;
        public int Height;

        public CropOptions(int x, int y, int width, int height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
        }
    }
}