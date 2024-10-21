#pragma once
#ifndef UNIASSET_RING_QUEUE_BUFFER_HPP
#define UNIASSET_RING_QUEUE_BUFFER_HPP

#include <cstdint>
#include <cstddef>

namespace uniasset {
    class RingQueueBuffer {
    public:
        RingQueueBuffer(size_t capacity);

        ~RingQueueBuffer();

        size_t write(const uint8_t* data, size_t count);

        size_t read(uint8_t* data, size_t count);

        size_t skip(size_t count);

        void resize(size_t newCapacity, bool copy);
        
        void clear();

        size_t size() const;

        size_t capacity() const;

    private:
        uint8_t* ringBuffer_;
        size_t capacity_;
        size_t head_;
        size_t tail_;
        size_t size_;
    };
}

#endif //UNIASSET_RINGQUEUEBUFFER_HPP
