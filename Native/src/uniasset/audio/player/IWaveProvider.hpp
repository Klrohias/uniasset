#pragma once

#ifndef UNIASSET_IWAVEPROVIDER_H
#define UNIASSET_IWAVEPROVIDER_H

#include <cstdint>

#include "uniasset/audio/SampleFormat.hpp"
#include "uniasset/audio/AudioConfig.hpp"

namespace uniasset {
	class IWaveProvider : public std::enable_shared_from_this<IWaveProvider> {
	public:
		struct ReadResult {
			void* data;
			uint32_t frameCount;
		};

		virtual void read(ReadResult& result, uint32_t frameOffset, uint32_t frameCount) = 0;

		virtual AudioConfig init(const AudioConfig& require) = 0;
	};
}

#endif // UNIASSET_AUDIOPLAYER_H