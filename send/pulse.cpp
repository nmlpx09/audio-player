#include "pulse.h"
#include "errors.h"

namespace NSend {

TSend::TSend(
    std::uint16_t bitsPerSample,
    std::uint8_t channels,
    std::uint32_t rate,
    std::string
)
: BitsPerSample(std::move(bitsPerSample))
, Channels(channels)
, Rate(rate) {
}

TSend::TSend(TSend&& pulse) noexcept {
    std::swap(BitsPerSample, pulse.BitsPerSample);
    std::swap(Channels, pulse.Channels);
    std::swap(Rate, pulse.Rate);
    std::swap(Simple, pulse.Simple);
    std::swap(Spec, pulse.Spec);
    std::swap(BufferAttr, pulse.BufferAttr);
}

TSend::~TSend() {
    if (Simple != nullptr) {
        pa_simple_drain(Simple, nullptr);
        pa_simple_free(Simple);
    }
}

std::error_code TSend::Init() noexcept {
    pa_sample_format_t format;
    if (BitsPerSample == 24) {
        format = PA_SAMPLE_S24LE;
    } else {
        return EErrorCode::DeviceInit;
    }

    if (Rate != 48000) {
        return EErrorCode::DeviceInit;
    }

    if (Channels != 2) {
        return EErrorCode::DeviceInit;
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
        return EErrorCode::DeviceInit;
    }

    return {};
}

std::error_code TSend::Send(TData&& data) noexcept {
    if (Simple == nullptr) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    pa_simple_write(Simple, data.data(), data.size(), nullptr);

    return {};
}

}
