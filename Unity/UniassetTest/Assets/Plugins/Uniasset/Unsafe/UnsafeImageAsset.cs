using System;
using System.Runtime.InteropServices;
using System.Text;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;

namespace Uniasset.Unsafe
{
    public readonly unsafe struct UnsafeImageAsset
    {
        public readonly void* Instance;

        public static UnsafeImageAsset Create()
        {
            return new UnsafeImageAsset(Interop.Uniasset_ImageAsset_Create());
        }
        
        public UnsafeImageAsset(void* instance)
        {
            Instance = instance;

            if (Instance == null)
                throw new Exception("Failed to create ImageAsset instance");
        }

        public int GetWidth()
        {
            var result = Interop.Uniasset_ImageAsset_GetWidth(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public int GetHeight()
        {
            var result = Interop.Uniasset_ImageAsset_GetHeight(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public int GetChannelCount()
        {
            var result = Interop.Uniasset_ImageAsset_GetChannelCount(Instance);
            CheckNativeErrorInternal();
            return result;
        }

        public void LoadFile(string path)
        {
            var pathBytes = Encoding.Default.GetBytes(path);
            fixed (byte* pathPtr = pathBytes)
            {
                Interop.Uniasset_ImageAsset_LoadFile(Instance, (sbyte*)pathPtr);
                CheckNativeErrorInternal();
            }
        }

        public void LoadMemory(Span<byte> data)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_Load(Instance, imageData, Convert.ToUInt64(data.Length));
                CheckNativeErrorInternal();
            }
        }

        public void LoadPixels(Span<byte> data, int width, int height, int channelCount)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_LoadPixels(Instance, imageData, Convert.ToUInt64(data.Length),
                    width, height, channelCount);
                CheckNativeErrorInternal();
            }
        }

        public void Unload()
        {
            Interop.Uniasset_ImageAsset_Unload(Instance);
            CheckNativeErrorInternal();
        }

        public void Clip(int x, int y, int width, int height)
        {
            Interop.Uniasset_ImageAsset_Clip(Instance, x, y, width, height);
            CheckNativeErrorInternal();
        }
        
        public void Resize(int width, int height)
        {
            Interop.Uniasset_ImageAsset_Resize(Instance, width, height);
            CheckNativeErrorInternal();
        }

        public void CopyTo(NativeArray<byte> dest)
        {
            var arrayPtr = dest.GetUnsafePtr();
            Interop.Uniasset_ImageAsset_CopyTo(Instance, arrayPtr);
            CheckNativeErrorInternal();
        }
        
        public void Destroy()
        {
            Interop.Uniasset_ImageAsset_Destory(Instance);
        }

        private void CheckNativeErrorInternal()
        {
            var errorMessage = Marshal.PtrToStringAnsi(new IntPtr(Interop.Uniasset_ImageAsset_GetError(Instance)));

            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new NativeException(errorMessage);
        }
    }
}