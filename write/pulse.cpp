#include "pulse.h"
#include "errors.h"

namespace NWrite {

TWrite::TWrite(std::string) { }

TWrite::TWrite(TWrite&& pulse) noexcept {
    std::swap(Simple, pulse.Simple);
    std::swap(Spec, pulse.Spec);
    std::swap(BufferAttr, pulse.BufferAttr);
}

TWrite::~TWrite() {
    if (Simple != nullptr) {
        pa_simple_drain(Simple, nullptr);
        pa_simple_free(Simple);
    }
}

std::error_code TWrite::Init(TFormat sampleFormat) noexcept {
    pa_sample_format_t format;

    if (!TFormat::NumChannelsPermited.contains(sampleFormat.NumChannels)) {
        return EErrorCode::DeviceInit;
    }

    if (!TFormat::BitsPerSamplePermited.contains(sampleFormat.BitsPerSample)) {
        return EErrorCode::DeviceInit;
    }

    if (sampleFormat.BitsPerSample == 24) {
        format = PA_SAMPLE_S24LE;
    } else if (sampleFormat.BitsPerSample == 16) {
        format = PA_SAMPLE_S16LE;
    } else if (sampleFormat.BitsPerSample == 32) {
        format = PA_SAMPLE_S32LE;
    } else {
        return EErrorCode::DeviceInit;
    }

    if (!TFormat::SampleRatePermited.contains(sampleFormat.SampleRate)) {
        return EErrorCode::DeviceInit;
    }

    Spec = {
        .format = format,
        .rate = sampleFormat.SampleRate,
        .channels = static_cast<std::uint8_t>(sampleFormat.NumChannels)
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

std::error_code TWrite::Write(const TCallback& callback) noexcept {
    TFormat currentFormat;

    while (true) {
        if (auto data = callback(); !data) {
            break;
        } else {
            auto&& [format, buffer] = data.value();

            if (currentFormat != format) {
                if (auto ec = Init(format); ec) {
                    return ec;
                } else {
                    currentFormat = format;
                }
            }

            pa_simple_write(Simple, buffer.data(), buffer.size(), nullptr);
        }
    }
    return {};
}

}
