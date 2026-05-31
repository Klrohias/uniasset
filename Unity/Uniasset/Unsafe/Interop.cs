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

        // Audio bindings intentionally left unchanged for now (per request),
        // but note: the current Rust FFI no longer exports these symbols.
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioAsset_Create();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_LoadFile(void* self, [NativeTypeName("const char *")] sbyte* path);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Load(void* self, [NativeTypeName("uint8_t *")] byte* data, [NativeTypeName("uint64_t")] ulong size);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Unload(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_AudioAsset_GetChannelCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint64_t")]
        public static extern ulong Uniasset_AudioAsset_GetSampleCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_AudioAsset_GetSampleRate(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern float Uniasset_AudioAsset_GetLength(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint8_t")]
        public static extern byte Uniasset_AudioAsset_GetLoadType(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("const char *")]
        public static extern sbyte* Uniasset_AudioAsset_GetPath(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioAsset_GetData(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint64_t")]
        public static extern ulong Uniasset_AudioAsset_GetDataLength(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioAsset_GetAudioDecoder(void* self, [NativeTypeName("uint8_t")] byte format, [NativeTypeName("int64_t")] long frameBufferSize);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_IAudioDecoder_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint8_t")]
        public static extern byte Uniasset_IAudioDecoder_GetSampleFormat(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_IAudioDecoder_GetChannelCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint64_t")]
        public static extern ulong Uniasset_IAudioDecoder_GetSampleCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_IAudioDecoder_GetSampleRate(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint32_t")]
        public static extern uint Uniasset_IAudioDecoder_Read(void* self, void* buffer, [NativeTypeName("uint32_t")] uint count);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint8_t")]
        public static extern byte Uniasset_IAudioDecoder_Seek(void* self, [NativeTypeName("int64_t")] long pos);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("int64_t")]
        public static extern long Uniasset_IAudioDecoder_Tell(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_FlacDecoder_Create(void* asset, [NativeTypeName("uint8_t")] byte format);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_OggDecoder_Create(void* asset, [NativeTypeName("uint8_t")] byte format);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_WavDecoder_Create(void* asset, [NativeTypeName("uint8_t")] byte format);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_Mp3Decoder_Create(void* asset, [NativeTypeName("uint8_t")] byte format);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_ExternalAudioDecoder_Create(void* userData, void* getChannelCountFunc, void* getSampleCountFunc, void* getSampleFormatFunc, void* getSampleRateFunc, void* readFunc, void* seekFunc, void* tellFunc);
    }
}
