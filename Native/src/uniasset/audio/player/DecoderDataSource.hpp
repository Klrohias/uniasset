#ifndef UNIASSET_DECODERDATASOURCE_H
#define UNIASSET_DECODERDATASOURCE_H
#include <miniaudio.h>

#include "uniasset/audio/IAudioDecoder.hpp"
#include "uniasset/common/Result.hpp"


namespace uniasset {
    class DecoderDataSource {
    public:
        DecoderDataSource(const std::shared_ptr<IAudioDecoder>& decoder);

        ~DecoderDataSource();

        static Result<std::unique_ptr<DecoderDataSource>> create(const std::shared_ptr<IAudioDecoder>& decoder);

        [[nodiscard]] ma_uint64 getSampleCount() const;

        [[nodiscard]] ma_uint32 getSampleRate() const;

        [[nodiscard]] ma_uint32 getChannelCount() const;

    private:
        std::error_code init();

        void uninit();

        static ma_result maRead(ma_data_source* ptr_data_source, void* ptr_frames_out, ma_uint64 frame_count, ma_uint64* p_frames_read);
        static ma_result maSeek(ma_data_source* ptr_data_source, ma_uint64 frame_index);
        static ma_result maGetDataFormat(ma_data_source* ptr_data_source, ma_format* ptr_format, ma_uint32* ptr_channels, ma_uint32* ptr_sample_rate, ma_channel* ptr_channel_map, size_t channel_map_cap);
        static ma_result maGetCursor(ma_data_source* ptr_data_source, ma_uint64* ptr_cursor_out);
        static ma_result maGetLength(ma_data_source* ptr_data_source, ma_uint64* ptr_length_out);
        static ma_result maSetLooping(ma_data_source* ptr_data_source, ma_bool32 is_looping);

        ma_data_source_base data_source_base_{};
        std::shared_ptr<IAudioDecoder> decoder_;
        static ma_data_source_vtable decoder_data_source_vtable_;
    };
}

#endif //UNIASSET_DECODERDATASOURCE_H
