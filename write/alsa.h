#pragma once

#include "interface.h"

#include <alsa/asoundlib.h>

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NWrite {

struct TWrite: TInterface {
public:
    TWrite(
        std::uint16_t bitsPerSample,
        std::uint8_t channels,
        std::uint32_t rate,
        std::string device
    );
    TWrite(const TWrite&) = delete;
    TWrite(TWrite&&) noexcept;
    ~TWrite();
    TWrite& operator=(const TWrite&) = delete;
    TWrite& operator=(TWrite&&) = delete;

    std::error_code Init() noexcept override;
    std::error_code Write(TData&& data) noexcept override;

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
