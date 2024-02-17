//
// Created by qingy on 2024/2/16.
//

// this file is written by me when I am sleep,
// there may be room for optimization

#include "BufferedAudioDecoder.hpp"

#include <utility>
#include <cstring>
#include <chrono>

namespace uniasset {
    using namespace std::chrono;
    using namespace std::chrono_literals;

    constexpr int64_t bufferBlockSize = 2048; // unit: frames
    constexpr auto bufferTimeThreshold = 2ms;

    inline uint8_t* getBufferOffset(std::unique_ptr<uint8_t[]>& buffer, int64_t frameSize, int64_t frame) {
        return buffer.get() + frameSize * frame;
    }

    BufferedAudioDecoder::BufferedAudioDecoder(std::shared_ptr<IAudioDecoder> wrappedAudioDecoder, int64_t bufferFrames)
            : wrappedAudioDecoder_{std::move(wrappedAudioDecoder)},
              sync_lock{std::make_unique<std::mutex>()} {

        if (bufferFrames == -1) {
            bufferFrames = wrappedAudioDecoder_->getSampleRate() * 8; // 8s
        }

        bufferSize_ = bufferFrames;
        frameSize_ = wrappedAudioDecoder_->getSampleFormat() == SampleFormat_Int16 ? sizeof(int16_t) : sizeof(float);
        frameSize_ *= wrappedAudioDecoder_->getChannelCount();

        buffer_ = std::make_unique<uint8_t[]>(bufferFrames * frameSize_);

        generateBufferImmediately();
    }

    int64_t BufferedAudioDecoder::tell() {
        return bufferedFrameBegin_;
    }

    bool BufferedAudioDecoder::seek(int64_t position) {
        std::lock_guard<std::mutex> lockGuard{*sync_lock};

        if (position == bufferedFrameBegin_) return true;

        if (bufferedFrameBegin_ <= position && position <= bufferedFrameEnd_) {
            auto delta = position - bufferedFrameBegin_;
            bufferedFrameBegin_ = delta;
            bufferOffset_ += delta;
            bufferOffset_ %= bufferSize_;

        } else {
            bufferedFrameBegin_ = position;
            bufferedFrameEnd_ = position;
            bufferOffset_ = 0;
        }

        wrappedAudioDecoder_->seek(position);
        generateBufferImmediately();
        return true;
    }

    void BufferedAudioDecoder::generateBuffer() {
        auto begin = system_clock::now();
        do {
            auto bufferedCount = bufferedFrameEnd_ - bufferedFrameBegin_;
            auto willBufferedCount = bufferSize_ - bufferedCount;

            if (willBufferedCount < bufferBlockSize) break;

            auto writeBegin = (bufferOffset_ + bufferedCount) % bufferSize_;

            if (writeBegin >= bufferOffset_) {
                auto writeSize = bufferSize_ - writeBegin;

                if (writeSize < bufferBlockSize) {
                    wrappedAudioDecoder_->read(getBufferOffset(buffer_, frameSize_, writeBegin), writeSize);
                    wrappedAudioDecoder_->read(getBufferOffset(buffer_, frameSize_, 0), bufferBlockSize - writeSize);
                } else {
                    wrappedAudioDecoder_->read(getBufferOffset(buffer_, frameSize_, writeBegin), bufferBlockSize);
                }

            } else {
                wrappedAudioDecoder_->read(getBufferOffset(buffer_, frameSize_, writeBegin), bufferBlockSize);
            }

            bufferedFrameEnd_ += bufferBlockSize;
        } while (system_clock::now() - begin < bufferTimeThreshold);
    }

    uint32_t BufferedAudioDecoder::getSampleRate() {
        return wrappedAudioDecoder_->getSampleRate();
    }

    SampleFormat BufferedAudioDecoder::getSampleFormat() {
        return wrappedAudioDecoder_->getSampleFormat();
    }

    size_t BufferedAudioDecoder::getSampleCount() {
        return wrappedAudioDecoder_->getSampleCount();
    }

    uint32_t BufferedAudioDecoder::getChannelCount() {
        return wrappedAudioDecoder_->getChannelCount();
    }

    bool BufferedAudioDecoder::read(void* buffer, uint32_t count) {
        std::lock_guard<std::mutex> lockGuard{*sync_lock};

        while (count > 0) {
            auto bufferedCount = bufferedFrameEnd_ - bufferedFrameBegin_;

            if (bufferedCount > count) {
                readInternal(buffer, count);
                bufferedFrameBegin_ += count;
                count -= count;
                generateBuffer();
            } else {
                readInternal(buffer, bufferedCount);
                bufferedFrameBegin_ += bufferedCount;
                count -= bufferedCount;

                generateBufferImmediately();
            }
        }

        return true;
    }

    void BufferedAudioDecoder::readInternal(void* buffer, int64_t requiredFrame) {
        auto readBegin = bufferOffset_;
        auto readEnd = (bufferOffset_ + requiredFrame) % bufferSize_;

        if (readBegin < readEnd) {
            memcpy(buffer, getBufferOffset(buffer_, frameSize_, readBegin), requiredFrame * frameSize_);
            bufferOffset_ = readEnd;
        } else {
            memcpy(buffer, getBufferOffset(buffer_, frameSize_, readBegin),
                   (bufferSize_ - readBegin) * frameSize_);
            memcpy(buffer, getBufferOffset(buffer_, frameSize_, readBegin),
                   readEnd * frameSize_);
            bufferOffset_ = readEnd;
        }
    }

    void BufferedAudioDecoder::generateBufferImmediately() {
        auto bufferedCount = bufferedFrameEnd_ - bufferedFrameBegin_;
        auto willBufferedCount = bufferSize_ - bufferedCount;
        if (willBufferedCount == 0) return;

        auto writeBegin = (bufferOffset_ + bufferedCount) % bufferSize_;

        if (writeBegin >= bufferOffset_) {
            wrappedAudioDecoder_->read(getBufferOffset(buffer_, frameSize_, writeBegin), bufferSize_ - writeBegin);
            writeBegin = 0;
        }

        wrappedAudioDecoder_->read(getBufferOffset(buffer_, frameSize_, writeBegin), bufferOffset_ - writeBegin);
        bufferedFrameEnd_ += willBufferedCount;
    }
} // uniasset
