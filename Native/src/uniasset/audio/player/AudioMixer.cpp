#include "AudioMixer.hpp"
#include <cstring>
#include <limits>
#include <algorithm>

#include "uniasset/common/Templates.hpp"

namespace uniasset {
	template <typename TSrc, typename TBuf>
	void mix_sample(AudioMixer* pMixer, const IWaveProvider::ReadResult& waveReadResult) {
		auto readFrameCount = (std::min)(pMixer->frameCount_, waveReadResult.frameCount);
		if (readFrameCount <= 0) {
			return;
		}

		pMixer->ensureBufferCapacity(sizeof(TBuf) * pMixer->frameCount_ * pMixer->bytesPerFrame_);

		TBuf* pMixBuffer = reinterpret_cast<TBuf*>(pMixer->pBuffer);
		TSrc* pSrc = reinterpret_cast<TSrc*>(waveReadResult.data);
		auto sampleCount = readFrameCount * pMixer->channelCount_;

		if (!pMixer->isBufferUsed_) {
			for (auto i = 0; i < sampleCount; i++) {
				pMixBuffer[i] = (TBuf)pSrc[i];
			}
			// pad with zero
			memset(ptr_offset(pMixBuffer, sizeof(TBuf) * sampleCount), 0, sizeof(TBuf) * (pMixer->frameCount_ - readFrameCount) * pMixer->channelCount_);
			pMixer->isBufferUsed_ = true;
		} else {
			for (auto i = 0; i < sampleCount; i++) {
				pMixBuffer[i] += (TBuf)pSrc[i];
			}
		}
	}

	void unsupport_mix(AudioMixer* pMixer, const IWaveProvider::ReadResult& waveReadResult) {
		// do nothing
	}

	template <typename TSrc, typename TBuf>
	void mix_sample_end(AudioMixer* pMixer, TBuf minv, TBuf maxv) {
		if (!pMixer->isBufferUsed_) {
			return;
		}
		
		// mix master buffer to mix buffer
		IWaveProvider::ReadResult result{pMixer->pDst_, pMixer->frameCount_ - pMixer->uncleanFrameCount_};
		pMixer->mixSample(pMixer, result);

		// copy mix result to master buffer
		TSrc* pMaster = reinterpret_cast<TSrc*>(pMixer->pDst_);
		TBuf* pMixBuffer = reinterpret_cast<TBuf*>(pMixer->pBuffer);

		auto sampleCount = pMixer->frameCount_ * pMixer->channelCount_;
		for (auto i = 0; i < sampleCount; i++) {
			pMaster[i] = static_cast<TSrc>(std::clamp(pMixBuffer[i], minv, maxv));
		}
	}

	void unsupport_mix_end(AudioMixer* pMixer) {
		// do nothing
	}

	template <typename TSrc, typename TBuf>
	inline void mix_integer_sample_end(AudioMixer* pMixer) {
		mix_sample_end<TSrc, TBuf>(pMixer, static_cast<TBuf>((std::numeric_limits<TSrc>::min)()), static_cast<TBuf>((std::numeric_limits<TSrc>::max)()));
	}

	template <typename TFP>
	inline void mix_fp_sample_end(AudioMixer* pMixer) {
		mix_sample_end<TFP, TFP>(pMixer, static_cast<TFP>(-1.0), static_cast<TFP>(1.0));
	}

	void unsupport_clamp(AudioMixer* pMixer) {
		// do nothing
	}

	AudioMixer::AudioMixer() = default;

	AudioMixer::AudioMixer(ma_format waveFormat, uint32_t channel) {
		this->bytesPerFrame_ = this->bytesPerSample_ * channel;
		this->bytesPerSample_ = ma_get_bytes_per_sample(waveFormat);
		this->channelCount_ = channel;

		mixSampleFunc mixers[] = {
			unsupport_mix,
			mix_sample<uint8_t, int16_t>,
			mix_sample<int16_t, int32_t>,
			unsupport_mix,
			mix_sample<int32_t, int64_t>,
			mix_sample<float, float>
		};
		mixSampleEndFunc endMixers[] = {
			unsupport_clamp,
			mix_integer_sample_end<uint8_t, int16_t>,
			mix_integer_sample_end<int16_t, int32_t>,
			unsupport_clamp,
			mix_integer_sample_end<int32_t, int64_t>,
			mix_fp_sample_end<float>
		};

		this->mixSample = mixers[waveFormat];
		this->mixSampleEnd = endMixers[waveFormat];
	}

	AudioMixer::~AudioMixer()
	{
		if (this->pBuffer != nullptr) {
			free(this->pBuffer);
		}
		pDst_ = nullptr;
		pBuffer = nullptr;
		mixSample = nullptr;
		mixSampleEnd = nullptr;
	}

	void AudioMixer::begin(void* pDst, uint32_t frameCount) {
		this->pDst_ = pDst;
		this->frameCount_ = frameCount;
		this->uncleanFrameCount_ = frameCount;
	}

	void AudioMixer::mix(const IWaveProvider::ReadResult& waveReadResult) {
		if (waveReadResult.frameCount <= 0) {
			return;
		}

		if (this->uncleanFrameCount_ == this->frameCount_) {
			// first wave provider, pad and copy wave data, skip mix
			auto readFrameCount = (std::min)(this->frameCount_, waveReadResult.frameCount);
			memcpy(pDst_, waveReadResult.data, readFrameCount * this->bytesPerFrame_);  // copy wave data
			this->uncleanFrameCount_ -= readFrameCount;
			return;
		}

		// have to mix with other wave provider data
		this->mixSample(this, waveReadResult);
	}

	void AudioMixer::end() {
		memset(ptr_offset(this->pDst_, this->bytesPerFrame_ * (this->frameCount_ - this->uncleanFrameCount_)), 0, this->frameCount_ * this->uncleanFrameCount_);
		this->mixSampleEnd(this);
	}

	void AudioMixer::ensureBufferCapacity(size_t size) {
		if (this->bufferSize_ >= size) {
			return;
		}
		if (this->pBuffer != nullptr) {
			free(this->pBuffer);
		}
		pBuffer = malloc(size);
		this->bufferSize_ = size;
		this->isBufferUsed_ = false;
	}
}