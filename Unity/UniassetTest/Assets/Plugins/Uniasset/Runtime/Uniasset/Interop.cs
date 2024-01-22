using System;
using System.Runtime.InteropServices;

namespace Uniasset
{
    public static class Interop
    {
#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN || UNITY_EDITOR || UNITY_STANDALONE_OSX || UNITY_STANDALONE_LINUX || UNITY_ANDROID
        const string LibraryName = "libuniasset";
#elif UNITY_IOS || UNITY_WEBGL
		const string LibraryName = "__Internal";
#endif

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_ImageAsset_Create();

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_Free(IntPtr obj);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_LoadFile(IntPtr self, IntPtr path);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_Load(IntPtr self, IntPtr data, ulong size);

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_ImageAsset_GetError(IntPtr self);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_LoadPixels(IntPtr self,
            IntPtr pixels,
            ulong size,
            int width,
            int height,
            int channelCount);

        [DllImport(LibraryName)]
        public static extern int Uniasset_ImageAsset_GetWidth(IntPtr self);

        [DllImport(LibraryName)]
        public static extern int Uniasset_ImageAsset_GetHeight(IntPtr self);

        [DllImport(LibraryName)]
        public static extern int Uniasset_ImageAsset_GetChannelCount(IntPtr self);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_Clip(IntPtr self, int x, int y, int width, int height);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_Resize(IntPtr self, int width, int height);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_CopyTo(IntPtr self, IntPtr dest);

        [DllImport(LibraryName)]
        public static extern void Uniasset_ImageAsset_Unload(IntPtr self);

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_ImageAsset_Clone(IntPtr self);

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_AudioAsset_Create();

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioAsset_Free(IntPtr obj);

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_AudioAsset_GetError(IntPtr self);

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioAsset_LoadFile(IntPtr self, IntPtr path);

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioAsset_Load(IntPtr self, IntPtr data, ulong size);

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioAsset_Unload(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern uint Uniasset_AudioAsset_GetChannelCount(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern uint Uniasset_AudioAsset_GetSampleRate(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern ulong Uniasset_AudioAsset_GetSampleCount(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern float Uniasset_AudioAsset_GetLength(IntPtr self);

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_AudioPlayer_Create();

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioPlayer_Free(IntPtr obj);

        [DllImport(LibraryName)]
        public static extern IntPtr Uniasset_AudioPlayer_GetError(IntPtr self);

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioPlayer_Open(IntPtr self, IntPtr audioAsset);

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioPlayer_Resume(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioPlayer_Pause(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioPlayer_Close(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern byte Uniasset_AudioPlayer_IsPaused(IntPtr self);

        [DllImport(LibraryName)]
        public static extern void Uniasset_AudioPlayer_SetVolume(IntPtr self, float val);
        
        [DllImport(LibraryName)]
        public static extern float Uniasset_AudioPlayer_GetVolume(IntPtr self);
        
        [DllImport(LibraryName)]
        public static extern float Uniasset_AudioPlayer_GetTime(IntPtr self);
    }
}