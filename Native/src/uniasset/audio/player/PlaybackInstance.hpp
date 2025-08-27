#pragma once
#ifndef UNIASSET_PLAYBACKINSTANCE_H
#define UNIASSET_PLAYBACKINSTANCE_H
#include <memory>

#include "DecoderDataSource.hpp"
#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/common/Result.hpp"

namespace uniasset {
    class AudioEngine;

    class UNIASSET_API PlaybackInstance : public std::enable_shared_from_this<PlaybackInstance> {
    public:
        static Result<std::unique_ptr<PlaybackInstance>> create(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<IAudioDecoder>& decoder, uint32_t flags);

        static Result<std::unique_ptr<PlaybackInstance>> create(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<DecoderDataSource>& source, uint32_t flags);

        ~PlaybackInstance();

        std::error_code init(uint32_t flags);

        void uninit();

        [[nodiscard]] Result<float> volume() const;

        void setVolume(float volume);

        [[nodiscard]] float time() const;

        std::error_code setTime(float time);

        [[nodiscard]] ma_uint64 frameTime() const;

        std::error_code setFrameTime(ma_uint64 frame);

        std::error_code play();

        std::error_code stop();

        [[nodiscard]] bool isPlaying() const;

        std::error_code playScheduled(ma_uint64 frame);

        void stopScheduled(ma_uint64 frame);

        [[nodiscard]] bool isLooping() const;

        void setLooping(bool loop);

    private:
        explicit PlaybackInstance(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<IAudioDecoder>& decoder);

        explicit PlaybackInstance(const std::shared_ptr<AudioEngine>& engine, const std::shared_ptr<DecoderDataSource>& source);

        std::shared_ptr<AudioEngine> engine_;
        std::shared_ptr<DecoderDataSource> source_;
        ma_sound sound_ {};
    };
} // uniasset

#endif //UNIASSET_PLAYBACKINSTANCE_H
