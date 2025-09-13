#include "pulse.h"
#include "errors.h"

namespace NSend {

TPulse::TPulse(
    std::uint16_t bitsPerSample,
    std::uint8_t channels,
    std::uint32_t rate
)
: BitsPerSample(std::move(bitsPerSample))
, Channels(channels)
, Rate(rate) {
}

TPulse::TPulse(TPulse&& pulse) noexcept {
    std::swap(BitsPerSample, pulse.BitsPerSample);
    std::swap(Channels, pulse.Channels);
    std::swap(Rate, pulse.Rate);
    std::swap(Simple, pulse.Simple);
    std::swap(Spec, pulse.Spec);
}

TPulse::~TPulse() {
    if (Simple) {
        pa_simple_drain(Simple, nullptr);
        pa_simple_free(Simple);
    }
}

std::error_code TPulse::Init() noexcept {
    pa_sample_format_t format;
    if (BitsPerSample == 16) {
        format = PA_SAMPLE_S16LE;
    } else {
        return EErrorCode::Format;
    }

    if (Rate != 48000) {
        return EErrorCode::Rate;
    }

    if (Channels != 2) {
        return EErrorCode::Channels;
    }

    Spec = {
        .format = format,
        .rate = Rate,
        .channels = Channels
    };

    BufferAttr = {
        .maxlength = static_cast<std::uint32_t>(-1),
        .tlength = static_cast<std::uint32_t>(pa_usec_to_bytes(1000000, &Spec)),
        .prebuf = static_cast<std::uint32_t>(-1),
        .minreq = static_cast<std::uint32_t>(-1),
        .fragsize = static_cast<std::uint32_t>(-1)
    };

    if (Simple = pa_simple_new(nullptr, "play", PA_STREAM_PLAYBACK, nullptr, "playback", &Spec, nullptr, &BufferAttr, nullptr); !Simple) {
        return EErrorCode::Prepare;
    }

    return {};
}

std::error_code TPulse::Send(TData&& data) noexcept {
    if (Simple == nullptr) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (pa_simple_write(Simple, data.data(), data.size(), nullptr) < 0) {
        return EErrorCode::Write;
    }

    return {};
}

}
