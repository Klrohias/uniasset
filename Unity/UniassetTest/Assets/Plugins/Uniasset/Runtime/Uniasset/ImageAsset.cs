using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Unity.Collections;
using Unity.Collections.LowLevel.Unsafe;
using UnityEngine;

namespace Uniasset
{
    public class ImageAsset : IDisposable, ICloneable
    {
        public IntPtr Handle { get; }

        private readonly CancellationTokenSource _cancellationTokenSource = new();

        public int Width
        {
            get
            {
                var result = Interop.Uniasset_ImageAsset_GetWidth(Handle);
                ThrowException();
                
                return result;
            }
        }
        
        public int Height
        {
            get
            {
                var result = Interop.Uniasset_ImageAsset_GetHeight(Handle);
                ThrowException();
                
                return result;
            }
        }
        
        public int ChannelCount
        {
            get
            {
                var result = Interop.Uniasset_ImageAsset_GetChannelCount(Handle);
                ThrowException();
                
                return result;
            }
        }
        
        public ImageAsset()
        {
            Handle = Interop.Uniasset_ImageAsset_Create();
            if (Handle == IntPtr.Zero)
            {
                throw new Exception("Failed to create ImageAsset instance");
            }
        }

        private ImageAsset(IntPtr handle)
        {
            Handle = handle;
        }
        
        public void Dispose()
        {
            _cancellationTokenSource.Cancel();
            Interop.Uniasset_ImageAsset_Free(Handle);
        }

        public void Load(string path)
        {
            var pathPtr = Marshal.StringToHGlobalAnsi(path);
            try
            {
                Interop.Uniasset_ImageAsset_LoadFile(Handle, pathPtr);
                ThrowException();
            }
            finally
            {
                Marshal.FreeHGlobal(pathPtr);
            }
        }

        public Task LoadAsync(string path)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Load(path);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public unsafe void Load(Span<byte> data)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_Load(Handle, new IntPtr(imageData), Convert.ToUInt64(data.Length));
                ThrowException();
            }
        }

        public Task LoadAsync(byte[] data)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Load(data);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public unsafe void Load(Span<byte> data, int width, int height, int channelCount)
        {
            fixed (byte* imageData = &data.GetPinnableReference())
            {
                Interop.Uniasset_ImageAsset_LoadPixels(Handle, new IntPtr(imageData), Convert.ToUInt64(data.Length),
                    width, height, channelCount);
                ThrowException();
            }
        }
        
        public Task LoadAsync(byte[] data, int width, int height, int channelCount)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Load(data, width, height, channelCount);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }
        
        public void Unload()
        {
            Interop.Uniasset_ImageAsset_Unload(Handle);
            ThrowException();
        }

        public void Clip(int x, int y, int width, int height)
        {
            Interop.Uniasset_ImageAsset_Clip(Handle, x, y, width, height);
            ThrowException();
        }

        public Task ClipAsync(int x, int y, int width, int height)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Clip(x, y, width, height);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public void Resize(int width, int height)
        {
            Interop.Uniasset_ImageAsset_Resize(Handle, width, height);
            ThrowException();
        }


        public Task ResizeAsync(int width, int height)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Resize(width, height);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public Texture2D ToTexture2D(bool mipmap = false, bool linear = true, bool noLongerReadable = true)
        {
            if (ChannelCount is < 3 or > 4)
            {
                throw new NotSupportedException($"Load a image with channel count {ChannelCount} is not supported");
            }

            var texture = new Texture2D(Width, Height, ChannelCount switch
            {
                3 => TextureFormat.RGB24,
                4 => TextureFormat.RGBA32
            }, mipmap, linear);

            var array = texture.GetRawTextureData<byte>();

            UnsafeToTexture2DInternal(array);
            
            texture.Apply(mipmap, noLongerReadable);
            
            return texture;
        }

        private unsafe void UnsafeToTexture2DInternal(NativeArray<byte> dest)
        {
            var arrayPtr = dest.GetUnsafePtr();
            Interop.Uniasset_ImageAsset_CopyTo(Handle, new IntPtr(arrayPtr));
            ThrowException();
        }
        
        public async Task<Texture2D> ToTexture2DAsync(bool mipmap = false, bool linear = true, bool noLongerReadable = true)
        {
            if (ChannelCount is < 3 or > 4)
            {
                throw new NotSupportedException($"Load a image with channel count {ChannelCount} is not supported");
            }

            var texture = new Texture2D(Width, Height, ChannelCount switch
            {
                3 => TextureFormat.RGB24,
                4 => TextureFormat.RGBA32,
                _ => throw new ArgumentOutOfRangeException()
            }, mipmap, linear);

            var array = texture.GetRawTextureData<byte>();
            
            await Task.Run(() =>
            {
                lock (this)
                {
                    UnsafeToTexture2DInternal(array);
                }
            }, _cancellationTokenSource.Token);
            
            texture.Apply(mipmap, noLongerReadable);

            return texture;
        }

        private void ThrowException()
        {
            var errorMessage = Marshal.PtrToStringAuto(Interop.Uniasset_ImageAsset_GetError(Handle));
            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new UniassetNativeException(errorMessage);
        }
        
        public ImageAsset Clone()
        {
            return new ImageAsset(Interop.Uniasset_ImageAsset_Clone(Handle));
        }
        
        object ICloneable.Clone()
        {
            return Clone();
        }
    }
}