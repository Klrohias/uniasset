using System;
using System.Threading;
using System.Threading.Tasks;
using Uniasset.Unsafe;
using UnityEngine;

namespace Uniasset.Image
{
    public sealed class ImageAsset : IDisposable, ICloneable
    {
        private bool _disposed = false;
        private readonly CancellationTokenSource _cancellationTokenSource = new();

        public UnsafeImageAsset UnsafeHandle { get; }

        public int Width => UnsafeHandle.GetWidth();

        public int Height => UnsafeHandle.GetHeight();

        public int ChannelCount => UnsafeHandle.GetChannelCount();

        public ImageAsset()
        {
            UnsafeHandle = UnsafeImageAsset.Create();
        }

        private ImageAsset(UnsafeImageAsset unsafeImageAsset)
        {
            UnsafeHandle = unsafeImageAsset;
        }

        public void Dispose()
        {
            if (_disposed) return;

            _disposed = true;
            _cancellationTokenSource.Cancel();
            UnsafeHandle.Destroy();
        }

        public void Load(string path)
        {
            UnsafeHandle.LoadFile(path);
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

        public void Load(Span<byte> data)
        {
            UnsafeHandle.LoadMemory(data);
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

        public void Load(Span<byte> data, int width, int height, int channelCount)
        {
            UnsafeHandle.LoadPixels(data, width, height, channelCount);
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
            UnsafeHandle.Unload();
        }

        public void Crop(int x, int y, int width, int height)
        {
            UnsafeHandle.Crop(x, y, width, height);
        }

        public Task CropAsync(int x, int y, int width, int height)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Crop(x, y, width, height);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public ImageAsset[] CropMultiple(CropOptions[] optionsArray)
        {
            var unsafeImageAssets = UnsafeHandle.CropMultiple(optionsArray);
            var result = new ImageAsset[optionsArray.Length];
            for (int i = 0; i < optionsArray.Length; i++)
            {
                result[i] = new ImageAsset(unsafeImageAssets[i]);
            }

            return result;
        }
        
        public Task<ImageAsset[]> CropMultipleAsync(CropOptions[] optionsArray)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    return CropMultiple(optionsArray);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public void Resize(int width, int height)
        {
            UnsafeHandle.Resize(width, height);
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
                throw new NotSupportedException($"Load a image with channel count {ChannelCount} is not supported");

            var texture = new Texture2D(Width, Height, ChannelCount switch
            {
                3 => TextureFormat.RGB24,
                4 => TextureFormat.RGBA32,
                _ => throw new ArgumentOutOfRangeException()
            }, mipmap, linear);

            var array = texture.GetRawTextureData<byte>();

            UnsafeHandle.CopyTo(array);

            texture.Apply(mipmap, noLongerReadable);

            return texture;
        }

        public async Task<Texture2D> ToTexture2DAsync(bool mipmap = false, bool linear = true,
            bool noLongerReadable = true)
        {
            if (ChannelCount is < 3 or > 4)
                throw new NotSupportedException($"Load a image with channel count {ChannelCount} is not supported");

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
                    UnsafeHandle.CopyTo(array);
                }
            }, _cancellationTokenSource.Token);

            texture.Apply(mipmap, noLongerReadable);

            return texture;
        }

        public ImageAsset Clone()
        {
            return new ImageAsset(UnsafeHandle.Clone());
        }

        object ICloneable.Clone() => Clone();

        ~ImageAsset()
        {
            Dispose();
        }
    }
}