#include "DecoderDataSource.hpp"

#include "uniasset/common/AudioUtils.hpp"
#include "uniasset/common/Errors.hpp"

namespace uniasset {
    ma_data_source_vtable DecoderDataSource::decoder_data_source_vtable_ = {
        maRead,
        maSeek,
        maGetDataFormat,
        maGetCursor,
        maGetLength,
        maSetLooping
    };

    Result<std::unique_ptr<DecoderDataSource>> DecoderDataSource::create(const std::shared_ptr<IAudioDecoder>& decoder) {
        const auto data_source = new DecoderDataSource(decoder);
        if (const auto err = data_source->init(); err != err_ok()) {
            delete data_source;
            return err;
        }
        return std::unique_ptr<DecoderDataSource>(data_source);
    }

    ma_uint64 DecoderDataSource::sampleCount() const {
        return decoder_->getSampleCount();
    }

    ma_uint32 DecoderDataSource::sampleRate() const {
        return decoder_->getSampleRate();
    }

    DecoderDataSource::DecoderDataSource(const std::shared_ptr<IAudioDecoder>& decoder) : decoder_(decoder) {
        data_source_base_.vtable = nullptr;
    }

    DecoderDataSource::~DecoderDataSource() {
        uninit();
    }

    std::error_code DecoderDataSource::init() {
        auto base_config = ma_data_source_config_init();
        base_config.vtable = &decoder_data_source_vtable_;

        const auto result = ma_data_source_init(&base_config, &data_source_base_);
        return result == MA_SUCCESS ? err_ok() : std::error_code{result, ma_category()};
    }

    void DecoderDataSource::uninit() {
        ma_data_source_uninit(&data_source_base_);
        data_source_base_.vtable = nullptr;
    }

    ma_result DecoderDataSource::maRead(ma_data_source* ptr_data_source, void* ptr_frames_out, ma_uint64 frame_count, ma_uint64* p_frames_read) {
        const auto data_source = static_cast<DecoderDataSource*>(ptr_data_source);
        *p_frames_read = data_source->decoder_->read(ptr_frames_out, frame_count);
        return MA_SUCCESS;
    }

    ma_result DecoderDataSource::maSeek(ma_data_source* ptr_data_source, ma_uint64 frame_index) {
        const auto data_source = static_cast<DecoderDataSource*>(ptr_data_source);
        const auto result = data_source->decoder_->seek(static_cast<int64_t>(frame_index));
        return result ? MA_SUCCESS : MA_OUT_OF_RANGE;
    }

    ma_result DecoderDataSource::maGetDataFormat(ma_data_source* ptr_data_source, ma_format* ptr_format, ma_uint32* ptr_channels, ma_uint32* ptr_sample_rate, ma_channel* ptr_channel_map, size_t channel_map_cap) {
        const auto data_source = static_cast<DecoderDataSource*>(ptr_data_source);
        *ptr_format = toMaFormat(data_source->decoder_->getSampleFormat());
        *ptr_channels = data_source->decoder_->getChannelCount();
        *ptr_sample_rate = data_source->decoder_->getSampleRate();
        if (*ptr_channels == 1 && channel_map_cap > 0) {
            ptr_channel_map[0] = MA_CHANNEL_MONO;
        }
        else if (*ptr_channels == 2 && channel_map_cap > 1) {
            ptr_channel_map[0] = MA_CHANNEL_LEFT;
            ptr_channel_map[1] = MA_CHANNEL_RIGHT;
        }
        else if (channel_map_cap > 0) {
            for (size_t i = 0; i < channel_map_cap; ++i) {
                ptr_channel_map[i] = MA_CHANNEL_NONE;
            }
        }
        return MA_SUCCESS;
    }

    ma_result DecoderDataSource::maGetCursor(ma_data_source* ptr_data_source, ma_uint64* ptr_cursor_out) {
        const auto data_source = static_cast<DecoderDataSource*>(ptr_data_source);
        *ptr_cursor_out = data_source->decoder_->tell();
        return MA_SUCCESS;
    }

    ma_result DecoderDataSource::maGetLength(ma_data_source* ptr_data_source, ma_uint64* ptr_length_out) {
        const auto data_source = static_cast<DecoderDataSource*>(ptr_data_source);
        *ptr_length_out = data_source->decoder_->getSampleCount();
        return MA_SUCCESS;
    }

    ma_result DecoderDataSource::maSetLooping(ma_data_source* ptr_data_source, ma_bool32 is_looping) {
        return MA_SUCCESS;
    }
}
