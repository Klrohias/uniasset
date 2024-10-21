#pragma once
#ifndef UNIASSET_BUFFERED_WAVE_PROVIDER_HPP
#define UNIASSET_BUFFERED_WAVE_PROVIDER_HPP

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "../IWaveProvider.hpp"
#include "uniasset/audio/AudioConfig.hpp"
#include "uniasset/common/RingQueueBuffer.hpp"
#include "uniasset/common/Buffer.hpp"

namespace uniasset {
	class BufferedWaveProvider : public IWaveProvider {
	public:
		BufferedWaveProvider(std::shared_ptr<IWaveProvider> upstream, uint32_t sampleCount);

		~BufferedWaveProvider();

		void read(ReadResult& result, uint32_t frameOffset, uint32_t frameCount) override;

		AudioConfig init(const AudioConfig& require) override;

		void readFromUpstream();

	private:
		AudioConfig audioConfig_;
		std::shared_ptr<IWaveProvider> upstream_;
		uint32_t framePostion_ = 0;
		RingQueueBuffer ringBuffer_;
		uint8_t* readBuffer_ = nullptr;
		size_t readBufferSize_ = 0;
		volatile bool isReading_ = false;

		void ensureReadBuffer(size_t size);
	};
}

#endif // UNIASSET_BUFFERED_WAVE_PROVIDER_HPP