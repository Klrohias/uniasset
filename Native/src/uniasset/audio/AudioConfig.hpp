#pragma once
#ifndef UNIASSET_AUDIO_CONFIG_HPP
#define UNIASSET_AUDIO_CONFIG_HPP

#include <cstdint>
#include "SampleFormat.hpp"

namespace uniasset {
	struct AudioConfig {
		uint32_t channelNum;
		SampleFormat sampleFormat;
		uint32_t sampleRate;

		AudioConfig() = default;

		AudioConfig(uint32_t channelNum, SampleFormat sampleFormat, uint32_t sampleRate)
			: channelNum(channelNum), sampleFormat(sampleFormat), sampleRate(sampleRate) { }
	};
}

#endif // UNIASSET_AUDIO_CONFIG_HPP