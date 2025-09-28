#pragma once

#include "interface.h"

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NRead {

namespace {

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

}

struct TWav: TInterface {
public:
    TWav() = default;
    TWav(const TWav&) = delete;
    TWav(TWav&&) noexcept = default;
    ~TWav();
    TWav& operator=(const TWav&) = delete;
    TWav& operator=(TWav&&) = delete;

    std::expected<TSampleFormat, std::error_code> Init(std::string fileName, std::size_t delay) noexcept override;
    std::error_code Read(const TCallback&) noexcept override;

private:
    std::int32_t DataSize = 0;
    std::int32_t Fd = -1;
};

}
