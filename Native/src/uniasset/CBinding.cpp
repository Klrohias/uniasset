//
// Created by Twiiz on 2024/1/14.
//

#include "CBinding.h"

#include <span>
#include <memory>
#include <optional>
#include "image/ImageAsset.hpp"
#include "audio/AudioAsset.hpp"
#include "audio/IAudioDecoder.hpp"
#include "audio/FlacDecoder.hpp"
#include "audio/Mp3Decoder.hpp"
#include "audio/OggDecoder.hpp"
#include "audio/WavDecoder.hpp"
#include "audio/ExternalAudioDecoder.hpp"
#include "audio/player/AudioEngine.hpp"
#include "common/Errors.hpp"

using namespace uniasset;

namespace internal {
    class ThreadErrorCodeStore {
        std::optional<std::error_code> errorCodeInstance_;
        std::string error_{};
    public:
        void set(const std::error_code& instance) {
            errorCodeInstance_ = instance;
            error_ = std::string(instance.category().name()) + ": " + instance.message();;
        }

        void clear() {
            error_ = {};
            errorCodeInstance_.reset();
        }

        bool hasError() {
            if (!errorCodeInstance_.has_value())
                return false;

            auto instance = *errorCodeInstance_;

            return instance.value();
        }

        const std::string& getError() {
            return error_;
        }
    };
}

thread_local internal::ThreadErrorCodeStore currentErrorCodeStore;

template<typename T>
std::shared_ptr<T>* createInstance(T* item) {
    return new std::shared_ptr<T>(item);
}

template<typename T>
void destroyInstance(void* ref) {
    delete reinterpret_cast<std::shared_ptr<T>*>(ref);
}

template<typename T>
std::shared_ptr<T>& getInstance(void* ref) {
    return *reinterpret_cast<std::shared_ptr<T>*>(ref);
}

// Errors


UNIASSET_API CBINDING_CSTRING Uniasset_GetError() {
    return currentErrorCodeStore.getError().c_str();
}

UNIASSET_API CBINDING_BOOLEAN Uniasset_HasError() {
    return currentErrorCodeStore.hasError();
}

// ImageAsset

CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Create) {
    currentErrorCodeStore.clear();
    return createInstance<ImageAsset>(new ImageAsset);
}

CBINDING_METHOD(void, ImageAsset, Destory, CBINDING_TYPED_PTR(ImageAsset) obj) {
    currentErrorCodeStore.clear();
    destroyInstance<ImageAsset>(obj);
}

CBINDING_METHOD(void, ImageAsset, LoadFile, CBINDING_TYPED_PTR(ImageAsset) self, CBINDING_CSTRING path) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->load(path));
}

CBINDING_METHOD(void, ImageAsset, LoadPixels, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* pixels,
                uint64_t size, int32_t width, int32_t height, int32_t channelCount) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->load(pixels, size, width, height, channelCount));
}

CBINDING_METHOD(void, ImageAsset, Load, CBINDING_TYPED_PTR(ImageAsset) self, uint8_t* data,
                uint64_t size) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->load(data, size));
}

CBINDING_METHOD(int32_t, ImageAsset, GetWidth, CBINDING_TYPED_PTR(ImageAsset) self) {
    auto result = getInstance<ImageAsset>(self)->getWidth();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(int32_t, ImageAsset, GetHeight, CBINDING_TYPED_PTR(ImageAsset) self) {
    auto result = getInstance<ImageAsset>(self)->getHeight();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(int32_t, ImageAsset, GetChannelCount, CBINDING_TYPED_PTR(ImageAsset) self) {
    auto result = getInstance<ImageAsset>(self)->getChannelCount();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(void, ImageAsset, Crop, CBINDING_TYPED_PTR(ImageAsset) self, int32_t x, int32_t y,
                int32_t width, int32_t height) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->crop(x, y, width, height));
}

CBINDING_METHOD(void, ImageAsset, CropMultiple, CBINDING_TYPED_PTR(ImageAsset) self, void* items, int16_t count,
                void** output) {
    auto result = getInstance<ImageAsset>(self)->cropMultiple(std::span(reinterpret_cast<CropOptions*>(items), count));

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return;
    }

    auto& subAssets = **result.data();
    for (int i = 0; i < count; ++i) {
        output[i] = createInstance<ImageAsset>(new ImageAsset(std::move(subAssets[i])));
    }

    subAssets.clear();
}

CBINDING_METHOD(void, ImageAsset, Resize, CBINDING_TYPED_PTR(ImageAsset) self, int32_t width,
                int32_t height) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->resize(width, height));
}

CBINDING_METHOD(void, ImageAsset, Unload, CBINDING_TYPED_PTR(ImageAsset) self) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->unload());
}

CBINDING_METHOD(void, ImageAsset, CopyTo, CBINDING_TYPED_PTR(ImageAsset) self, void* dest) {
    currentErrorCodeStore.set(getInstance<ImageAsset>(self)->copyTo(dest));
}

CBINDING_METHOD(CBINDING_TYPED_PTR(ImageAsset), ImageAsset, Clone, CBINDING_TYPED_PTR(ImageAsset) self) {
    auto result = getInstance<ImageAsset>(self)->clone();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return nullptr;
    }

    return createInstance(**result.data());
}

// AudioAsset
CBINDING_METHOD(CBINDING_TYPED_PTR(AudioAsset), AudioAsset, Create) {
    currentErrorCodeStore.clear();
    return createInstance<AudioAsset>(new AudioAsset);
}

CBINDING_METHOD(void, AudioAsset, Destory, CBINDING_TYPED_PTR(AudioAsset) obj) {
    currentErrorCodeStore.clear();
    destroyInstance<AudioAsset>(obj);
}

CBINDING_METHOD(void, AudioAsset, LoadFile, CBINDING_TYPED_PTR(AudioAsset) self, CBINDING_CSTRING path) {
    currentErrorCodeStore.set(getInstance<AudioAsset>(self)->load(path));
}

CBINDING_METHOD(void, AudioAsset, Load, CBINDING_TYPED_PTR(AudioAsset) self, uint8_t* data, uint64_t size) {
    currentErrorCodeStore.set(getInstance<AudioAsset>(self)->load(std::span<uint8_t>(data, size)));
}

CBINDING_METHOD(void, AudioAsset, Unload, CBINDING_TYPED_PTR(AudioAsset) self) {
    currentErrorCodeStore.clear();
    getInstance<AudioAsset>(self)->unload();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetChannelCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    auto result = getInstance<AudioAsset>(self)->getChannelCount();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(uint64_t, AudioAsset, GetSampleCount, CBINDING_TYPED_PTR(AudioAsset) self) {
    auto result = getInstance<AudioAsset>(self)->getSampleCount();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(uint32_t, AudioAsset, GetSampleRate, CBINDING_TYPED_PTR(AudioAsset) self) {
    auto result = getInstance<AudioAsset>(self)->getSampleRate();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(float, AudioAsset, GetLength, CBINDING_TYPED_PTR(AudioAsset) self) {
    auto result = getInstance<AudioAsset>(self)->getLength();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(uint8_t, AudioAsset, GetLoadType, CBINDING_TYPED_PTR(AudioAsset) self) {
    currentErrorCodeStore.clear();
    return getInstance<AudioAsset>(self)->getLoadType();
}

CBINDING_METHOD(CBINDING_CSTRING, AudioAsset, GetPath, CBINDING_TYPED_PTR(AudioAsset) self) {
    auto result = getInstance<AudioAsset>(self)->getPath();

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return nullptr;
    }

    return (**result.data()).data();
}

CBINDING_METHOD(void*, AudioAsset, GetData, CBINDING_TYPED_PTR(AudioAsset) self) {
    currentErrorCodeStore.clear();
    return getInstance<AudioAsset>(self)->getData().get();
}

CBINDING_METHOD(uint64_t, AudioAsset, GetDataLength, CBINDING_TYPED_PTR(AudioAsset) self) {
    currentErrorCodeStore.clear();
    return getInstance<AudioAsset>(self)->getDataLength();
}

CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), AudioAsset, GetAudioDecoder, CBINDING_TYPED_PTR(AudioAsset) self,
                uint8_t format, int64_t frameBufferSize) {
    auto result = getInstance<AudioAsset>(self)->getAudioDecoder(static_cast<SampleFormat>(format), frameBufferSize);

    currentErrorCodeStore.clear();
    if (auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return nullptr;
    }

    return createInstance<IAudioDecoder>((**result.data()).release());
}

// IAudioDecoder
CBINDING_METHOD(void, IAudioDecoder, Destory, CBINDING_TYPED_PTR(IAudioDecoder) obj) {
    currentErrorCodeStore.clear();
    destroyInstance<IAudioDecoder>(obj);
}

CBINDING_METHOD(uint8_t, IAudioDecoder, GetSampleFormat, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->getSampleFormat();
}

CBINDING_METHOD(uint32_t, IAudioDecoder, GetChannelCount, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->getChannelCount();
}

CBINDING_METHOD(uint64_t, IAudioDecoder, GetSampleCount, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->getSampleCount();
}

CBINDING_METHOD(uint32_t, IAudioDecoder, GetSampleRate, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->getSampleRate();
}

CBINDING_METHOD(uint32_t, IAudioDecoder, Read, CBINDING_TYPED_PTR(IAudioDecoder) self, void* buffer,
                uint32_t count) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->read(buffer, count);
}

CBINDING_METHOD(CBINDING_BOOLEAN, IAudioDecoder, Seek, CBINDING_TYPED_PTR(IAudioDecoder) self, int64_t pos) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->seek(pos);
}

CBINDING_METHOD(int64_t, IAudioDecoder, Tell, CBINDING_TYPED_PTR(IAudioDecoder) self) {
    currentErrorCodeStore.clear();
    return getInstance<IAudioDecoder>(self)->tell();
}

// FlacDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), FlacDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    currentErrorCodeStore.clear();
    return createInstance<IAudioDecoder>(new FlacDecoder(getInstance<AudioAsset>(asset),
                                                         static_cast<SampleFormat>(format)));
}

// OggDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), OggDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    currentErrorCodeStore.clear();
    return createInstance<IAudioDecoder>(new OggDecoder(getInstance<AudioAsset>(asset),
                                                        static_cast<SampleFormat>(format)));
}

// WavDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), WavDecoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    currentErrorCodeStore.clear();
    return createInstance<IAudioDecoder>(new WavDecoder(getInstance<AudioAsset>(asset),
                                                        static_cast<SampleFormat>(format)));
}

// Mp3Decoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), Mp3Decoder, Create, CBINDING_TYPED_PTR(AudioAsset) asset,
                uint8_t format) {
    currentErrorCodeStore.clear();
    return createInstance<IAudioDecoder>(new Mp3Decoder(getInstance<AudioAsset>(asset),
                                                        static_cast<SampleFormat>(format)));
}

// ExternalAudioDecoder
CBINDING_METHOD(CBINDING_TYPED_PTR(IAudioDecoder), ExternalAudioDecoder, Create, void* userData,
                void* getChannelCountFunc,
                void* getSampleCountFunc,
                void* getSampleFormatFunc,
                void* getSampleRateFunc,
                void* readFunc,
                void* seekFunc,
                void* tellFunc) {
    currentErrorCodeStore.clear();
    return createInstance<IAudioDecoder>(
            new ExternalAudioDecoder(userData,
                                     reinterpret_cast<ExternalAudioDecoder::GetChannelCountFunc>(getChannelCountFunc),
                                     reinterpret_cast<ExternalAudioDecoder::GetSampleCountFunc>(getSampleCountFunc),
                                     reinterpret_cast<ExternalAudioDecoder::GetSampleFormatFunc>(getSampleFormatFunc),
                                     reinterpret_cast<ExternalAudioDecoder::GetSampleRateFunc>(getSampleRateFunc),
                                     reinterpret_cast<ExternalAudioDecoder::ReadFunc>(readFunc),
                                     reinterpret_cast<ExternalAudioDecoder::SeekFunc>(seekFunc),
                                     reinterpret_cast<ExternalAudioDecoder::TellFunc>(tellFunc)));
}

CBINDING_METHOD(CBINDING_TYPED_PTR(AudioEngine), AudioEngine, Create) {
    auto result = AudioEngine::create();

    currentErrorCodeStore.clear();
    if (const auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return nullptr;
    }

    return createInstance<AudioEngine>((*result.data())->release());
}

CBINDING_METHOD(void, AudioEngine, Destory, CBINDING_TYPED_PTR(AudioEngine) obj) {
    currentErrorCodeStore.clear();
    destroyInstance<AudioEngine>(obj);
}

CBINDING_METHOD(float, AudioEngine, GetVolume, CBINDING_TYPED_PTR(AudioEngine) self) {
    currentErrorCodeStore.clear();
    return getInstance<AudioEngine>(self)->volume();
}

CBINDING_METHOD(void, AudioEngine, SetVolume, CBINDING_TYPED_PTR(AudioEngine) self, float volume) {
    currentErrorCodeStore.set(getInstance<AudioEngine>(self)->setVolume(volume));
}

CBINDING_METHOD(uint64_t, AudioEngine, GetTimeInPcmFrames, CBINDING_TYPED_PTR(AudioEngine) self) {
    currentErrorCodeStore.clear();
    return getInstance<AudioEngine>(self)->getTimeInPcmFrames();
}

CBINDING_METHOD(void, AudioEngine, ResetTimeInPcmFrames, CBINDING_TYPED_PTR(AudioEngine) self) {
    currentErrorCodeStore.set(getInstance<AudioEngine>(self)->resetTimeInPcmFrames());
}

CBINDING_METHOD(uint32_t, AudioEngine, GetSampleRate, CBINDING_TYPED_PTR(AudioEngine) self) {
    currentErrorCodeStore.clear();
    return getInstance<AudioEngine>(self)->getSampleRate();
}

CBINDING_METHOD(CBINDING_TYPED_PTR(PlaybackInstance), AudioEngine, CreatePlayback,
                CBINDING_TYPED_PTR(AudioEngine) self, CBINDING_TYPED_PTR(IAudioDecoder) decoder) {
    const auto engine = getInstance<AudioEngine>(self);
    const auto dec = getInstance<IAudioDecoder>(decoder);
    auto playback = engine->createPlayback(dec);

    currentErrorCodeStore.clear();
    if (const auto err = playback.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return nullptr;
    }
    return createInstance<PlaybackInstance>((**playback.data()).release());
}

CBINDING_METHOD(void, PlaybackInstance, Destory, CBINDING_TYPED_PTR(PlaybackInstance) obj) {
    currentErrorCodeStore.clear();
    destroyInstance<PlaybackInstance>(obj);
}

CBINDING_METHOD(float, PlaybackInstance, GetVolume, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    const auto playback = getInstance<PlaybackInstance>(self);
    auto result = playback->volume();

    currentErrorCodeStore.clear();
    if (const auto err = result.error(); err.has_value()) {
        currentErrorCodeStore.set(**err);
        return 0;
    }

    return **result.data();
}

CBINDING_METHOD(void, PlaybackInstance, SetVolume, CBINDING_TYPED_PTR(PlaybackInstance) self, float volume) {
    currentErrorCodeStore.clear();
    getInstance<PlaybackInstance>(self)->setVolume(volume);
}

CBINDING_METHOD(float, PlaybackInstance, GetTime, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    currentErrorCodeStore.clear();
    return getInstance<PlaybackInstance>(self)->time();
}

CBINDING_METHOD(void, PlaybackInstance, SetTime, CBINDING_TYPED_PTR(PlaybackInstance) self, float time) {
    currentErrorCodeStore.set(getInstance<PlaybackInstance>(self)->setTime(time));
}

CBINDING_METHOD(uint64_t, PlaybackInstance, GetFrameTime, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    currentErrorCodeStore.clear();
    return getInstance<PlaybackInstance>(self)->frameTime();
}

CBINDING_METHOD(void, PlaybackInstance, SetFrameTime, CBINDING_TYPED_PTR(PlaybackInstance) self, uint64_t frame) {
    currentErrorCodeStore.clear();
    if (const auto err = getInstance<PlaybackInstance>(self)->setFrameTime(frame); err != err_ok()) {
        currentErrorCodeStore.set(err);
    }
}

CBINDING_METHOD(void, PlaybackInstance, Play, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    currentErrorCodeStore.clear();
    if (const auto err = getInstance<PlaybackInstance>(self)->play(); err != err_ok()) {
        currentErrorCodeStore.set(err);
    }
}

CBINDING_METHOD(void, PlaybackInstance, Stop, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    currentErrorCodeStore.clear();
    if (const auto err = getInstance<PlaybackInstance>(self)->stop(); err != err_ok()) {
        currentErrorCodeStore.set(err);
    }
}

CBINDING_METHOD(CBINDING_BOOLEAN, PlaybackInstance, IsPlaying, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    currentErrorCodeStore.clear();
    return getInstance<PlaybackInstance>(self)->isPlaying();
}

CBINDING_METHOD(void, PlaybackInstance, PlayScheduled, CBINDING_TYPED_PTR(PlaybackInstance) self, uint64_t frame) {
    const auto playback = getInstance<PlaybackInstance>(self);
    if (const auto err = playback->playScheduled(frame); err != err_ok()) {
        currentErrorCodeStore.set(err);
    }
}

CBINDING_METHOD(void, PlaybackInstance, StopScheduled, CBINDING_TYPED_PTR(PlaybackInstance) self, uint64_t frame) {
    currentErrorCodeStore.clear();
    getInstance<PlaybackInstance>(self)->stopScheduled(frame);
}

CBINDING_METHOD(CBINDING_BOOLEAN, PlaybackInstance, IsLooping, CBINDING_TYPED_PTR(PlaybackInstance) self) {
    currentErrorCodeStore.clear();
    return getInstance<PlaybackInstance>(self)->isLooping();
}

CBINDING_METHOD(void, PlaybackInstance, SetLooping, CBINDING_TYPED_PTR(PlaybackInstance) self, CBINDING_BOOLEAN loop) {
    currentErrorCodeStore.clear();
    getInstance<PlaybackInstance>(self)->setLooping(loop);
}