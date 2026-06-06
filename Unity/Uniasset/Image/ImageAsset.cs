using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Uniasset;
using Uniasset.Unsafe;
using UnityEngine;

namespace Uniasset.Image
{
    public sealed class ImageAsset : IDisposable, ICloneable
    {
        private int _disposed;
        private readonly CancellationTokenSource _cancellationTokenSource = new();

        public UnsafeImageAsset UnsafeHandle { get; }

        public int Width => UnsafeHandle.GetWidth();

        public int Height => UnsafeHandle.GetHeight();

        public int ChannelCount => UnsafeHandle.GetChannelCount();

        public PixelType PixelType => UnsafeHandle.GetPixelType();

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
            if (Interlocked.CompareExchange(ref _disposed, 1, 0) != 0) return;

            _cancellationTokenSource.Cancel();
            _cancellationTokenSource.Dispose();
            UnsafeHandle.Destroy();
        }

        public void Load(string path, int expectedWidth = 0, int expectedHeight = 0)
        {
            if (expectedWidth < 0) throw new ArgumentOutOfRangeException(nameof(expectedWidth));
            if (expectedHeight < 0) throw new ArgumentOutOfRangeException(nameof(expectedHeight));
            UnsafeHandle.LoadFile(path, (uint)expectedWidth, (uint)expectedHeight);
        }

        public Task LoadAsync(string path, int expectedWidth = 0, int expectedHeight = 0)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Load(path, expectedWidth, expectedHeight);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public void Load(Span<byte> data, int expectedWidth = 0, int expectedHeight = 0)
        {
            if (expectedWidth < 0) throw new ArgumentOutOfRangeException(nameof(expectedWidth));
            if (expectedHeight < 0) throw new ArgumentOutOfRangeException(nameof(expectedHeight));
            UnsafeHandle.LoadMemory(data, (uint)expectedWidth, (uint)expectedHeight);
        }

        public void LoadIO(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
        {
            if (expectedWidth < 0) throw new ArgumentOutOfRangeException(nameof(expectedWidth));
            if (expectedHeight < 0) throw new ArgumentOutOfRangeException(nameof(expectedHeight));
            UnsafeHandle.LoadIO(stream, (uint)expectedWidth, (uint)expectedHeight);
        }

        public void LoadIO(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
        {
            LoadIO(new StreamWrapper(stream), expectedWidth, expectedHeight);
        }

        public Task LoadIOAsync(IUniassetStream stream, int expectedWidth = 0, int expectedHeight = 0)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    LoadIO(stream, expectedWidth, expectedHeight);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public Task LoadIOAsync(Stream stream, int expectedWidth = 0, int expectedHeight = 0)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    LoadIO(stream, expectedWidth, expectedHeight);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        public Task LoadAsync(byte[] data, int expectedWidth = 0, int expectedHeight = 0)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Load(data, expectedWidth, expectedHeight);
                }
            }, _cancellationTokenSource.Token, TaskCreationOptions.DenyChildAttach, TaskScheduler.Default);
        }

        // Removed: LoadPixels-style API. New native FFI only supports loading encoded image bytes.

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

        public void Resize(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
        {
            UnsafeHandle.Resize(width, height, filter);
        }

        public Task ResizeAsync(int width, int height, ResizeFilter filter = ResizeFilter.Nearest)
        {
            return Task.Factory.StartNew(() =>
            {
                lock (this)
                {
                    Resize(width, height, filter);
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
