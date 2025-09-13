#pragma once

#include "interface.h"

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NRead {

struct TWavHeader {
    char ChunkId[4];
    std::uint32_t ChunkSize;
    char Format[4];
    char SubChunk1Id[4];
    std::uint32_t SubChunk1Size;
    std::uint16_t AudioFormat;
    std::uint16_t NumChannels;
    std::uint32_t SampleRate;
    std::uint32_t ByteRate;
    std::uint16_t BlockAlign;
    std::uint16_t BitsPerSample;
    char SubChunk2Id[4];
    std::uint32_t SubChunk2Size;
};

struct TWav: TInterface {
public:
    TWav(
        std::string fileName,
        std::uint16_t bitsPerSample,
        std::uint8_t cannels,
        std::uint32_t rate,
        std::int32_t dataSize
    );
    TWav(const TWav&) = delete;
    TWav(TWav&&) noexcept = default;
    ~TWav();
    TWav& operator=(const TWav&) = delete;
    TWav& operator=(TWav&&) = delete;

    std::error_code Init() noexcept;
    std::expected<TData, std::error_code> Rcv() const noexcept;

private:
    std::string FileName;
    std::uint16_t BitsPerSample;
    std::uint8_t Cannels = 0;
    std::uint32_t Rate = 0;
    std::int32_t DataSize = 0;
    std::int32_t Fd = -1;
};

}
