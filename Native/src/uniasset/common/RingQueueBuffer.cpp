#include "RingQueueBuffer.hpp"
#include <algorithm>
#include <cstring>

namespace uniasset {
    RingQueueBuffer::RingQueueBuffer(size_t capacity)
        : capacity_(capacity), head_(0), tail_(0), size_(0) {
        ringBuffer_ = capacity == 0 ? nullptr : reinterpret_cast<uint8_t*>(malloc(capacity));
    }

    RingQueueBuffer::~RingQueueBuffer() {
        if (ringBuffer_ != nullptr)
            free(ringBuffer_);
    }

    size_t RingQueueBuffer::write(const uint8_t* data, size_t count) {
        size_t space = capacity_ - size_;
        size_t writeCount = std::min(count, space);

        if (writeCount == 0) {
            return 0;
        }

        size_t firstPart = std::min(writeCount, capacity_ - tail_);
        memcpy(ringBuffer_ + tail_, data, firstPart);
        memcpy(ringBuffer_, data + firstPart, writeCount - firstPart);

        tail_ = (tail_ + writeCount) % capacity_;
        size_ += writeCount;
    }

    size_t RingQueueBuffer::read(uint8_t* data, size_t count) {
        size_t readCount = std::min(count, size_);
        if (readCount == 0) {
            return 0;
        }

        size_t firstPart = std::min(readCount, capacity_ - head_);
        std::memcpy(data, ringBuffer_ + head_, firstPart);
        std::memcpy(data + firstPart, ringBuffer_, readCount - firstPart);

        head_ = (head_ + readCount) % capacity_;
        size_ -= readCount;

        return readCount;
    }

    size_t RingQueueBuffer::skip(size_t count) {
        size_t readCount = std::min(count, size_);
        head_ = (head_ + readCount) % capacity_;
        size_ -= readCount;
        return readCount;
    }

    void RingQueueBuffer::resize(size_t newCapacity, bool copy) {
        if (newCapacity == capacity_) {
            return; // No need to resize if the capacity is the same
        }

        if (newCapacity == 0) {
            if (ringBuffer_ != nullptr) {
                free(ringBuffer_);
            }
            ringBuffer_ = nullptr;
            capacity_ = size_ = 0;
            head_ = tail_ = 0;
            return;
        }

        uint8_t* newBuffer = reinterpret_cast<uint8_t*>(malloc(newCapacity));

        // Copy the data to the new buffer
        size_t newSize = copy ? std::min(size_, newCapacity) : 0;
        if (newSize > 0) {
            size_t firstPart = std::min(newSize, capacity_ - head_);
            memcpy(newBuffer, ringBuffer_ + head_, firstPart);
            memcpy(newBuffer + firstPart, ringBuffer_, newSize - firstPart);
        }

        if (ringBuffer_ != nullptr) {
            free(ringBuffer_);
        }
        ringBuffer_ = newBuffer;
        capacity_ = newCapacity;
        head_ = 0;
        tail_ = newSize % newCapacity;
        size_ = newSize;
    }

    void RingQueueBuffer::clear()
    {
        head_ = tail_ = size_ = 0;
    }

    size_t RingQueueBuffer::size() const {
        return size_;
    }

    size_t RingQueueBuffer::capacity() const {
        return capacity_;
    }
}
