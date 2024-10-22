#include "AdaptWaveProvider.hpp"
#include "uniasset/common/AudioUtils.hpp"

namespace uniasset {
	void AdaptWaveProvider::read(ReadResult& result, uint32_t frameOffset, uint32_t frameCount) {
		ReadResult upstreamResult;
		upstream_->read(upstreamResult, frameOffset, frameCount);
		result = upstreamResult;
	}

	AudioConfig AdaptWaveProvider::init(const AudioConfig& require) {
		AudioConfig inputConfig = upstream_->init(require);
		this->inputConfig_ = inputConfig;
		this->converterConfig_ = ma_data_converter_config_init(toMaFormat(inputConfig.sampleFormat), toMaFormat(require.sampleFormat),
			inputConfig.channelNum, require.channelNum, inputConfig.sampleRate, require.sampleRate);
		return require;
	}
}