using System;
using System.Runtime.InteropServices;
using System.Text;
using Uniasset;
using Uniasset.Image;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;

namespace Uniasset.Unsafe
{
    public readonly unsafe partial struct UnsafeImageAsset
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
            return checked((int)result);
        }

        public int GetHeight()
        {
            var result = Interop.Uniasset_ImageAsset_GetHeight(Instance);
            NativeException.ThrowIfNeeded();
            return checked((int)result);
        }

        public PixelType GetPixelType()
        {
            var result = (PixelType)Interop.Uniasset_ImageAsset_GetPixelType(Instance);
            NativeException.ThrowIfNeeded();
            return result;
        }

        public int GetChannelCount()
        {
            var pixelType = GetPixelType();
            return pixelType switch
            {
                PixelType.RGB => 3,
                PixelType.RGBA => 4,
                PixelType.ARGB => 4,
                PixelType.Grey => 1,
                _ => 0,
            };
        }

        public void LoadFile(string path, uint expectedWidth = 0, uint expectedHeight = 0)
        {
            var pathBytes = Encoding.UTF8.GetBytes(path + '\0');
            fixed (byte* pathPtr = pathBytes)
            {
                Interop.Uniasset_ImageAsset_LoadFile(Instance, (sbyte*)pathPtr, expectedWidth, expectedHeight);
                NativeException.ThrowIfNeeded();
            }
        }

        public void LoadMemory(Span<byte> data, uint expectedWidth = 0, uint expectedHeight = 0)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_Load(
                    Instance,
                    imageData,
                    Convert.ToUInt64(data.Length),
                    expectedWidth,
                    expectedHeight);
                NativeException.ThrowIfNeeded();
            }
        }

        public void Unload()
        {
            Interop.Uniasset_ImageAsset_Unload(Instance);
            NativeException.ThrowIfNeeded();
        }

        public void LoadIO(IUniassetStream stream, uint expectedWidth = 0, uint expectedHeight = 0)
        {
            if (stream == null) throw new ArgumentNullException(nameof(stream));

            var gcHandle = GCHandle.Alloc(stream);
            try
            {
                var provider = Interop.NativeIOProvider.Default();
                provider.userData = GCHandle.ToIntPtr(gcHandle).ToPointer();

                Interop.Uniasset_ImageAsset_LoadIO(Instance, &provider, expectedWidth, expectedHeight);
                NativeException.ThrowIfNeeded();
            }
            finally
            {
                gcHandle.Free();
            }
        }

        public UnsafeImageAsset Clone()
        {
            var result = Interop.Uniasset_ImageAsset_Clone(Instance);
            NativeException.ThrowIfNeeded();
            return new UnsafeImageAsset(result);
        }

        public void Crop(int x, int y, int width, int height)
        {
            Interop.Uniasset_ImageAsset_Crop(
                Instance,
                checked((uint)x),
                checked((uint)y),
                checked((uint)width),
                checked((uint)height));
            NativeException.ThrowIfNeeded();
        }

        public UnsafeImageAsset[] CropMultiple(CropOptions[] optionsArray)
        {
            fixed (CropOptions* options = optionsArray)
            fixed (void** result = new void*[optionsArray.Length])
            {
                Interop.Uniasset_ImageAsset_CropMultiple(Instance, options, (uint)optionsArray.Length, result);
                NativeException.ThrowIfNeeded();

                var handleResult = new UnsafeImageAsset[optionsArray.Length];
                for (int i = 0; i < optionsArray.Length; i++)
                {
                    handleResult[i] = new UnsafeImageAsset(result[i]);
                }

                return handleResult;
            }
        }
        
        public void Resize(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
        {
            Interop.Uniasset_ImageAsset_Resize(
                Instance,
                checked((uint)width),
                checked((uint)height),
                (uint)filter);
            NativeException.ThrowIfNeeded();
        }

        public void CopyTo(NativeArray<byte> dest)
        {
            var arrayPtr = (byte*)dest.GetUnsafePtr();
            Interop.Uniasset_ImageAsset_CopyTo(Instance, arrayPtr, (ulong)dest.Length);
            NativeException.ThrowIfNeeded();
        }

        public void Destroy()
        {
            Interop.Uniasset_ImageAsset_Destory(Instance);
        }
    }
}
