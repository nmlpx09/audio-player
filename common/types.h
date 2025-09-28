#pragma once

#include <cstdint>
#include <vector>

using TData = std::vector<std::uint8_t>;

struct TSampleFormat {
    std::uint32_t BytesPerSample = 0;
    std::uint32_t NumChannels = 0;
    std::uint32_t SampleRate = 0;

    bool operator ==(const TSampleFormat& lhs) const {
        return BytesPerSample == lhs.BytesPerSample && NumChannels == lhs.NumChannels && SampleRate == lhs.SampleRate;
    }
};
