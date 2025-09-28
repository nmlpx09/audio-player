#pragma once

#include "interface.h"

#include <common/types.h>

#include "FLAC++/decoder.h"

#include <cstdint>
#include <list>
#include <string>

namespace NRead {

namespace {

struct TFlacDecoder: FLAC::Decoder::File {
public:
    FLAC__StreamDecoderWriteStatus write_callback(const FLAC__Frame* frame, const FLAC__int32* const buffer[]) override;
    void metadata_callback(const FLAC__StreamMetadata* metadata) override;
    void error_callback(FLAC__StreamDecoderErrorStatus status) override;

    TCallback Callback;
    std::size_t DataSize = 0;
    std::uint8_t NumChannels = 0;
    std::uint16_t BytesPerSample = 0;
    std::uint32_t SampleRate = 0;

    std::list<TData::value_type> Buffer;
};

}

struct TFlac: TInterface {
public:
    TFlac();
    TFlac(const TFlac&) = delete;
    TFlac(TFlac&&) noexcept = delete;
    ~TFlac();
    TFlac& operator=(const TFlac&) = delete;
    TFlac& operator=(TFlac&&) = delete;

    std::expected<TSampleFormat, std::error_code> Init(std::string fileName, std::size_t delay) noexcept override;
    std::error_code Read(const TCallback&) noexcept override;

private:
    TFlacDecoder Decoder;
};

}
