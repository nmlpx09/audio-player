#pragma once

#include "interface.h"

#include <alsa/asoundlib.h>

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NSend {

struct TSend: TInterface {
public:
    TSend(
        std::uint16_t bitsPerSample,
        std::uint8_t channels,
        std::uint32_t rate,
        std::string device
    );
    TSend(const TSend&) = delete;
    TSend(TSend&&) noexcept;
    ~TSend();
    TSend& operator=(const TSend&) = delete;
    TSend& operator=(TSend&&) = delete;

    std::error_code Init() noexcept override;
    std::error_code Send(TData&& data) noexcept override;

private:
    std::uint16_t BitsPerSample = 0;
    std::uint8_t Channels = 0;
    std::uint32_t Rate = 0;
    std::string Device;

    snd_pcm_t* SoundDevice = nullptr;
    snd_pcm_hw_params_t* HwParams = nullptr;
    std::size_t FrameSize = 1;
};

}
