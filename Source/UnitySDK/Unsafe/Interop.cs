using System.Runtime.InteropServices;

namespace Uniasset.Unsafe
{
    public static unsafe partial class Interop
    {
        // Core error handling
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("const char *")]
        public static extern sbyte* Uniasset_GetError();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint8_t")]
        public static extern byte Uniasset_HasError();

        // ImageAsset
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_ImageAsset_Create();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_LoadFile(
            void* self,
            [NativeTypeName("const char *")] sbyte* path,
            [NativeTypeName("uint32_t")] uint expectedWidth,
            [NativeTypeName("uint32_t")] uint expectedHeight);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Load(
            void* self,
            [NativeTypeName("const uint8_t *")] byte* data,
            [NativeTypeName("uint64_t")] ulong size,
            [NativeTypeName("uint32_t")] uint expectedWidth,
            [NativeTypeName("uint32_t")] uint expectedHeight);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_LoadIO(
            void* self,
            NativeIOProvider* provider,
            [NativeTypeName("uint32_t")] uint expectedWidth,
            [NativeTypeName("uint32_t")] uint expectedHeight);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_ImageAsset_GetWidth(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_ImageAsset_GetHeight(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_ImageAsset_GetPixelType(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Crop(
            void* self,
            [NativeTypeName("uint32_t")] uint l,
            [NativeTypeName("uint32_t")] uint t,
            [NativeTypeName("uint32_t")] uint w,
            [NativeTypeName("uint32_t")] uint h);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Resize(
            void* self,
            [NativeTypeName("uint32_t")] uint w,
            [NativeTypeName("uint32_t")] uint h,
            [NativeTypeName("uint32_t")] uint filter);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Unload(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_CopyTo(
            void* self,
            [NativeTypeName("uint8_t *")] byte* dest,
            [NativeTypeName("uint64_t")] ulong size);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_ImageAsset_Clone(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_CropMultiple(
            void* self,
            void* items,
            [NativeTypeName("uint32_t")] uint count,
            void** output);

        // AudioAsset
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioAsset_Create();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_LoadFile(void* self, [NativeTypeName("const char *")] sbyte* path, [NativeTypeName("uint8_t")] byte sampleFormat);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_LoadMemory(void* self, [NativeTypeName("const uint8_t *")] byte* data, [NativeTypeName("size_t")] ulong size, [NativeTypeName("uint8_t")] byte sampleFormat);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_LoadIO(void* self, NativeIOProvider* provider, [NativeTypeName("uint8_t")] byte sampleFormat);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint16_t")]
        public static extern ushort Uniasset_AudioAsset_GetChannelCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint64_t")]
        public static extern ulong Uniasset_AudioAsset_GetSampleCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("unsigned int")]
        public static extern uint Uniasset_AudioAsset_GetSampleRate(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint64_t")]
        public static extern ulong Uniasset_AudioAsset_GetFrameCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("int64_t")]
        public static extern long Uniasset_AudioAsset_Tell(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("unsigned int")]
        public static extern uint Uniasset_AudioAsset_Read(void* self, [NativeTypeName("uint8_t *")] byte* buffer, [NativeTypeName("size_t")] ulong bufferSize, [NativeTypeName("unsigned int")] uint frameCount);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Unload(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Seek(void* self, [NativeTypeName("int64_t")] long position);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("unsigned int")]
        public static extern uint Uniasset_AudioAsset_ReadUnsafe(void* self, [NativeTypeName("uint8_t *")] byte* buffer, [NativeTypeName("size_t")] ulong bufferSize, [NativeTypeName("unsigned int")] uint frameCount);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_SeekUnsafe(void* self, [NativeTypeName("int64_t")] long position);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Prepare(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioAsset_TryClone(void* self);
    }
}
