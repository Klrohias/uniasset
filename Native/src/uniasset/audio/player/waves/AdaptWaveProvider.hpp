#pragma once
#ifndef UNIASSET_ADAPT_WAVE_PROVIDER_H
#define UNIASSET_ADAPT_WAVE_PROVIDER_H
#include <memory>

#include "../IWaveProvider.hpp"
#include "miniaudio.h"

namespace uniasset {
	class AdaptWaveProvider : public IWaveProvider {
	public:
		~AdaptWaveProvider() = default;

		void read(ReadResult& result, uint32_t frameOffset, uint32_t frameCount) override;

		AudioConfig init(const AudioConfig& require) override;

	private:
		std::shared_ptr<IWaveProvider> upstream_{ 0 };
		ma_data_converter_config converterConfig_;
		AudioConfig inputConfig_;
	};
}

#endif // UNIASSET_ADAPT_WAVE_PROVIDER_H