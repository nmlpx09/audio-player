#pragma once

#include "interface.h"

#include <pulse/simple.h>

#include <common/types.h>

#include <cstdint>
#include <string>

namespace NSend {

struct TPulse: TInterface {
public:
    TPulse(
        std::uint16_t bitsPerSample,
        std::uint8_t channels,
        std::uint32_t rate
    );
    TPulse(const TPulse&) = delete;
    TPulse(TPulse&&) noexcept;
    ~TPulse();
    TPulse& operator=(const TPulse&) = delete;
    TPulse& operator=(TPulse&&) = delete;

    std::error_code Init() noexcept override;
    std::error_code Send(TData&& data) noexcept override;

private:
    std::uint16_t BitsPerSample;
    std::uint8_t Channels = 0;
    std::uint32_t Rate = 0;
    
    pa_simple* Simple = nullptr;
    pa_sample_spec Spec;
    pa_buffer_attr BufferAttr;
};

}
