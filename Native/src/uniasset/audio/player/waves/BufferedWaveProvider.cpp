#include "BufferedWaveProvider.hpp"
#include <uniasset/common/AudioUtils.hpp>

namespace uniasset {
	BufferedWaveProvider::BufferedWaveProvider(std::shared_ptr<IWaveProvider> upstream, uint32_t sampleCount)
		: audioConfig_(), upstream_(upstream), framePostion_(0), ringBuffer_(0), readBuffer_(nullptr), readBufferSize_(0) {}

	BufferedWaveProvider::~BufferedWaveProvider() {
		if (readBuffer_ != nullptr) {
			free(readBuffer_);
		}
	}

	void BufferedWaveProvider::read(ReadResult& result, uint32_t frameOffset, uint32_t frameCount) {
		size_t frameSize = calcFrameSize(audioConfig_.sampleFormat, audioConfig_.channelNum);
		ensureReadBuffer(frameSize * frameCount);
		uint32_t frameDelta = frameOffset - framePostion_;

		isReading_ = true;
		if (frameDelta < 0) {
			// clear data in buffer and return directly
			framePostion_ = frameOffset;
			ringBuffer_.clear();
			upstream_->read(result, frameOffset, frameCount);
			isReading_ = false;
			return;
		}
		if (frameDelta > 0) {
			// skip unused frame
			size_t skipByteCount = frameSize * frameDelta;
			ringBuffer_.skip(skipByteCount);
			framePostion_ += frameDelta;
		}

		uint32_t bufferFrameCount = ringBuffer_.size() / frameSize;
		size_t readByteCount = ringBuffer_.read(readBuffer_, frameSize * bufferFrameCount);
		result.data = readBuffer_;
		result.frameCount = readByteCount / frameSize;
		framePostion_ += result.frameCount;
		isReading_ = false;
	}

	AudioConfig BufferedWaveProvider::init(const AudioConfig& require) {
		audioConfig_ = upstream_->init(require);
		size_t bufferSize = time2FrameOffset(4.0f, audioConfig_.sampleRate) * calcFrameSize(audioConfig_.sampleFormat, audioConfig_.channelNum);
		ringBuffer_.resize(bufferSize, false);
		return audioConfig_;
	}

	void BufferedWaveProvider::readFromUpstream() {
		if (isReading_) {
			return;
		}

		size_t readByteCount = time2FrameOffset(1.0f, audioConfig_.sampleRate) * calcFrameSize(audioConfig_.sampleFormat, audioConfig_.channelNum);
		if (ringBuffer_.capacity() - ringBuffer_.size() < readByteCount) {
			return;
		}

		size_t frameSize = calcFrameSize(audioConfig_.sampleFormat, audioConfig_.channelNum);
		ReadResult upstreamResult;
		upstream_->read(upstreamResult, framePostion_ + (ringBuffer_.size() / frameSize), readByteCount / frameSize);
	}

	void BufferedWaveProvider::ensureReadBuffer(size_t size) {
		if (this->readBufferSize_ >= size) {
			return;
		}

		if (this->readBuffer_ != nullptr) {
			free(this->readBuffer_);
		}

		this->readBuffer_ = (uint8_t*)malloc(size);
	}
}