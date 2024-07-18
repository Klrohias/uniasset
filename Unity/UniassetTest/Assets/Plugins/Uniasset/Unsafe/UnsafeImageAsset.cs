using System;
using System.Runtime.InteropServices;
using System.Text;
using Uniasset.Image;
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
            NativeException.ThrowIfNeeded();
            return result;
        }

        public int GetHeight()
        {
            var result = Interop.Uniasset_ImageAsset_GetHeight(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public int GetChannelCount()
        {
            var result = Interop.Uniasset_ImageAsset_GetChannelCount(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public void LoadFile(string path)
        {
            var pathBytes = Encoding.Default.GetBytes(path);
            fixed (byte* pathPtr = pathBytes)
            {
                Interop.Uniasset_ImageAsset_LoadFile(Instance, (sbyte*)pathPtr);
                NativeException.ThrowIfNeeded();
            }
        }

        public void LoadMemory(Span<byte> data)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_Load(Instance, imageData, Convert.ToUInt64(data.Length));
                NativeException.ThrowIfNeeded();
            }
        }

        public void LoadPixels(Span<byte> data, int width, int height, int channelCount)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_LoadPixels(Instance, imageData, Convert.ToUInt64(data.Length),
                    width, height, channelCount);
                NativeException.ThrowIfNeeded();
            }
        }

        public void Unload()
        {
            Interop.Uniasset_ImageAsset_Unload(Instance);
            NativeException.ThrowIfNeeded();
        }

        public UnsafeImageAsset Clone()
        {
            var result = Interop.Uniasset_ImageAsset_Clone(Instance);
            NativeException.ThrowIfNeeded();
            return new UnsafeImageAsset(result);
        }

        public void Crop(int x, int y, int width, int height)
        {
            Interop.Uniasset_ImageAsset_Crop(Instance, x, y, width, height);
            NativeException.ThrowIfNeeded();
        }
        
        public UnsafeImageAsset[] CropMultiple(CropOptions[] optionsArray)
        {
            fixed (CropOptions* options = optionsArray)
            fixed (void** result = new void*[optionsArray.Length])
            {
                Interop.Uniasset_ImageAsset_CropMultiple(Instance, options, (short)optionsArray.Length, result);
                NativeException.ThrowIfNeeded();

                var handleResult = new UnsafeImageAsset[optionsArray.Length];
                for (int i = 0; i < optionsArray.Length; i++)
                {
                    handleResult[i] = new UnsafeImageAsset(result[i]);
                }

                return handleResult;
            }
        }

        public void Resize(int width, int height)
        {
            Interop.Uniasset_ImageAsset_Resize(Instance, width, height);
            NativeException.ThrowIfNeeded();
        }

        public void CopyTo(NativeArray<byte> dest)
        {
            var arrayPtr = dest.GetUnsafePtr();
            Interop.Uniasset_ImageAsset_CopyTo(Instance, arrayPtr);
            NativeException.ThrowIfNeeded();
        }

        public void Destroy()
        {
            Interop.Uniasset_ImageAsset_Destory(Instance);
        }
    }
}