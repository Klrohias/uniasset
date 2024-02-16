namespace Uniasset.Unsafe
{
    public static class NativeLibrary
    {
#if UNITY_EDITOR_WIN || UNITY_STANDALONE_WIN
        public const string LibraryName = "uniasset";
#elif UNITY_STANDALONE_OSX || UNITY_STANDALONE_LINUX || UNITY_ANDROID
        public const string LibraryName = "libuniasset";
#elif UNITY_IOS || UNITY_WEBGL
		public const string LibraryName = "__Internal";
#endif
    }
}