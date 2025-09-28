#pragma once

#include <cstdint>
#include <vector>
#include <set>

using TData = std::vector<std::uint8_t>;

struct TSampleFormat {
    inline static std::set<std::uint32_t> SampleRatePermited = {44100, 48000, 96000, 192000};
    inline static std::set<std::uint32_t> BitsPerSamplePermited = {16, 24};
    inline static std::set<std::uint32_t> NumChannelsPermited = {2};

    std::uint32_t BitsPerSample = 0;
    std::uint32_t NumChannels = 0;
    std::uint32_t SampleRate = 0;

    bool operator ==(const TSampleFormat& lhs) const {
        return BitsPerSample == lhs.BitsPerSample && NumChannels == lhs.NumChannels && SampleRate == lhs.SampleRate;
    }
};
