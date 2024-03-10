using System.Runtime.InteropServices;

namespace Uniasset.Unsafe
{
    public static unsafe partial class Interop
    {
        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_ImageAsset_Create();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_LoadFile(void* self, [NativeTypeName("const char *")] sbyte* path);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_LoadPixels(void* self, [NativeTypeName("uint8_t *")] byte* pixels, [NativeTypeName("uint64_t")] ulong size, [NativeTypeName("int32_t")] int width, [NativeTypeName("int32_t")] int height, [NativeTypeName("int32_t")] int channelCount);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Load(void* self, [NativeTypeName("uint8_t *")] byte* data, [NativeTypeName("uint64_t")] ulong size);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("const char *")]
        public static extern sbyte* Uniasset_ImageAsset_GetError(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("int32_t")]
        public static extern int Uniasset_ImageAsset_GetWidth(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("int32_t")]
        public static extern int Uniasset_ImageAsset_GetHeight(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("int32_t")]
        public static extern int Uniasset_ImageAsset_GetChannelCount(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Crop(void* self, [NativeTypeName("int32_t")] int x, [NativeTypeName("int32_t")] int y, [NativeTypeName("int32_t")] int width, [NativeTypeName("int32_t")] int height);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Resize(void* self, [NativeTypeName("int32_t")] int width, [NativeTypeName("int32_t")] int height);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_Unload(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_CopyTo(void* self, void* dest);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_ImageAsset_Clone(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_ImageAsset_CropMultiple(void* self, void* items, [NativeTypeName("int16_t")] short count, void** output);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioPlayer_Create();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("const char *")]
        public static extern sbyte* Uniasset_AudioPlayer_GetError(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_Pause(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_Resume(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_Close(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_Open(void* self, void* audioAsset);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_OpenDecoder(void* self, void* audioDecoder);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("uint8_t")]
        public static extern byte Uniasset_AudioPlayer_IsPaused(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_SetVolume(void* self, float val);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern float Uniasset_AudioPlayer_GetVolume(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern float Uniasset_AudioPlayer_GetTime(void* self);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioPlayer_SetTime(void* self, float time);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void* Uniasset_AudioAsset_Create();

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern void Uniasset_AudioAsset_Destory(void* obj);

        [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        [return: NativeTypeName("const char *")]
        public static extern sbyte* Uniasset_AudioAsset_GetError(void* self);

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
