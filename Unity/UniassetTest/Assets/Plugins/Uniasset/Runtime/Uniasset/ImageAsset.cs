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
        private readonly IntPtr _handle;
        private readonly CancellationTokenSource _cancellationTokenSource = new();

        public int Width
        {
            get
            {
                var result = Interop.Uniasset_ImageAsset_GetWidth(_handle);
                
                if (result == -1) 
                    ThrowException();
                
                return result;
            }
        }
        
        public int Height
        {
            get
            {
                var result = Interop.Uniasset_ImageAsset_GetHeight(_handle);
                
                if (result == -1) 
                    ThrowException();
                
                return result;
            }
        }
        
        public int ChannelCount
        {
            get
            {
                var result = Interop.Uniasset_ImageAsset_GetChannelCount(_handle);
                
                if (result == -1) 
                    ThrowException();
                
                return result;
            }
        }
        
        public ImageAsset()
        {
            _handle = Interop.Uniasset_ImageAsset_Create();
            if (_handle == IntPtr.Zero)
            {
                throw new Exception("Failed to create ImageAsset instance");
            }
        }

        private ImageAsset(IntPtr handle)
        {
            _handle = handle;
        }
        
        public void Dispose()
        {
            _cancellationTokenSource.Cancel();
            Interop.Uniasset_ImageAsset_Free(_handle);
        }

        public void Load(string path)
        {
            var pathPtr = Marshal.StringToHGlobalAnsi(path);
            try
            {
                if (Interop.Uniasset_ImageAsset_LoadFile(_handle, pathPtr) == 0)
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
                if (Interop.Uniasset_ImageAsset_Load(_handle,
                        new IntPtr(imageData), Convert.ToUInt64(data.Length)) == 0)
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
                if (Interop.Uniasset_ImageAsset_LoadPixels(_handle,
                        new IntPtr(imageData), Convert.ToUInt64(data.Length), width, height, channelCount) == 0)
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
            if (Interop.Uniasset_ImageAsset_Unload(_handle) == 0)
                ThrowException();
        }

        public void Clip(int x, int y, int width, int height)
        {
            if (Interop.Uniasset_ImageAsset_Clip(_handle, x, y, width, height) == 0)
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
            if (Interop.Uniasset_ImageAsset_Resize(_handle, width, height) == 0)
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
            if (Interop.Uniasset_ImageAsset_CopyTo(_handle, new IntPtr(arrayPtr)) == 0)
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
                4 => TextureFormat.RGBA32
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
            var errorMessage = Marshal.PtrToStringAuto(Interop.Uniasset_ImageAsset_GetError(_handle));
            if (string.IsNullOrWhiteSpace(errorMessage)) return;
            throw new UniassetNativeException(errorMessage);
        }
        
        public ImageAsset Clone()
        {
            return new ImageAsset(Interop.Uniasset_ImageAsset_Clone(_handle));
        }
        
        object ICloneable.Clone()
        {
            return Clone();
        }
    }
}