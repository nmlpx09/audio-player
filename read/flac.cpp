#include "errors.h"
#include "flac.h"

namespace NRead {

void TFlacDecoder::metadata_callback(const FLAC__StreamMetadata *metadata) {
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        SampleRate = metadata->data.stream_info.sample_rate;
        NumChannels = metadata->data.stream_info.channels;
        BitsPerSample = metadata->data.stream_info.bits_per_sample;
    }
}

FLAC__StreamDecoderWriteStatus TFlacDecoder::write_callback(const FLAC__Frame* frame, const FLAC__int32* const frames[]) {
    if (!Callback) {
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    const auto bytesPerSample = BitsPerSample / 8;
    for (std::size_t index = 0; index < frame->header.blocksize; ++index) {
        auto* lelf = reinterpret_cast<const std::uint8_t*>(&frames[0][index]);
        for (std::size_t byteIndex = 0; byteIndex < bytesPerSample; ++byteIndex) {
            Buffer.push_back(lelf[byteIndex]);
        }

        auto* right = reinterpret_cast<const std::uint8_t*>(&frames[1][index]);
        for (std::size_t byteIndex = 0; byteIndex < bytesPerSample; ++byteIndex) {
            Buffer.push_back(right[byteIndex]);
        }
    }

    while (Buffer.size() >= DataSize) {
        TData result;
        result.reserve(DataSize);
        for (std::size_t index = 0; index < DataSize; ++index) {
            result.push_back(Buffer.front());
            Buffer.pop_front();
        }
        Callback(std::move(result));
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void TFlacDecoder::error_callback(FLAC__StreamDecoderErrorStatus) { }

TFlac::TFlac() {}

TFlac::~TFlac() {}

std::expected<TFormat, std::error_code> TFlac::Init(std::string fileName, std::size_t delay) noexcept {
    auto status = Decoder.init(fileName);

    if(status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        return std::unexpected(EErrorCode::FileOpen);
    }

    Decoder.process_until_end_of_metadata();

    if (!TFormat::NumChannelsPermited.contains(Decoder.NumChannels)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TFormat::SampleRatePermited.contains(Decoder.SampleRate)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TFormat::BitsPerSamplePermited.contains(Decoder.BitsPerSample)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    Decoder.DataSize =  Decoder.NumChannels * Decoder.SampleRate * (Decoder.BitsPerSample / 8) * delay / 1000;

    return TFormat {
        .BitsPerSample = Decoder.BitsPerSample,
        .NumChannels = Decoder.NumChannels,
        .SampleRate =  Decoder.SampleRate
    };
}

std::error_code TFlac::Read(const TCallback& callback) noexcept {
    Decoder.Callback = callback;

    Decoder.process_until_end_of_stream();

    return {};
}

}
