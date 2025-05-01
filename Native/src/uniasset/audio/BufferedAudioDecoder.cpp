//
// Created by qingy on 2024/2/16.
//

// this file is written by me when I am sleep,
// there may be room for optimization

#include "BufferedAudioDecoder.hpp"

#include <utility>
#include <cstring>
#include <chrono>
#include "uniasset/common/Templates.hpp"

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

        ringBuffer_ = std::make_unique<uint8_t[]>(bufferFrames * frameSize_);

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
                    readWrappedDecoder(getBufferOffset(ringBuffer_, frameSize_, writeBegin), writeSize);
                    readWrappedDecoder(getBufferOffset(ringBuffer_, frameSize_, 0), bufferBlockSize - writeSize);
                } else {
                    readWrappedDecoder(getBufferOffset(ringBuffer_, frameSize_, writeBegin), bufferBlockSize);
                }

            } else {
                readWrappedDecoder(getBufferOffset(ringBuffer_, frameSize_, writeBegin), bufferBlockSize);
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

    uint32_t BufferedAudioDecoder::read(void* buffer, uint32_t count) {
        std::lock_guard<std::mutex> lockGuard{*sync_lock};

        auto remain = count;
        while (remain > 0) {
            auto bufferedCount = bufferedFrameEnd_ - bufferedFrameBegin_;

            if (bufferedCount > remain) {
                readInternal(buffer, remain);
                bufferedFrameBegin_ += remain;
                remain -= remain;
                generateBuffer();
            } else {
                auto bufferOffset = (count - remain) * frameSize_;
                readInternal(ptr_offset(buffer, bufferOffset), bufferedCount);
                bufferedFrameBegin_ += bufferedCount;
                remain -= bufferedCount;
                generateBufferImmediately();
            }
        }

        return count;
    }

    void BufferedAudioDecoder::readInternal(void* buffer, int64_t requiredFrame) {
        auto readBegin = bufferOffset_;
        auto readEnd = (bufferOffset_ + requiredFrame) % bufferSize_;

        if (readBegin < readEnd) {
            memcpy(buffer, getBufferOffset(ringBuffer_, frameSize_, readBegin), requiredFrame * frameSize_);
            bufferOffset_ = readEnd;
        } else {
            auto offset = (bufferSize_ - readBegin) * frameSize_;
            memcpy(buffer, getBufferOffset(ringBuffer_, frameSize_, readBegin),
                   offset);
            memcpy(ptr_offset(buffer, offset), getBufferOffset(ringBuffer_, frameSize_, 0),
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
            readWrappedDecoder(getBufferOffset(ringBuffer_, frameSize_, writeBegin),
                               bufferSize_ - writeBegin);
            writeBegin = 0;
        }

        readWrappedDecoder(getBufferOffset(ringBuffer_, frameSize_, writeBegin),
                           bufferOffset_ - writeBegin);
        bufferedFrameEnd_ += willBufferedCount;
    }

    void BufferedAudioDecoder::readWrappedDecoder(void* buffer, int64_t requiredFrame) {
        if (uint32_t readFrameCount = wrappedAudioDecoder_->read(buffer, requiredFrame);
                readFrameCount < requiredFrame) {
            memset(ptr_offset(buffer, readFrameCount * frameSize_),
                   0,
                   (requiredFrame - readFrameCount) * frameSize_);
        }
    }
} // uniasset
