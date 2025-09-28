#pragma once

#include "interface.h"

#include <alsa/asoundlib.h>

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NWrite {

struct TWrite: TInterface {
public:
    TWrite(std::string device);
    TWrite(const TWrite&) = delete;
    TWrite(TWrite&&) noexcept;
    ~TWrite();
    TWrite& operator=(const TWrite&) = delete;
    TWrite& operator=(TWrite&&) = delete;
    std::error_code Write(const TCallback& callback) noexcept override;

private:
    std::error_code Init(TSampleFormat sampleFormat) noexcept override;

private:
    std::string Device;
    snd_pcm_t* SoundDevice = nullptr;
    snd_pcm_hw_params_t* HwParams = nullptr;
    std::size_t FrameSize = 1;
};

}
