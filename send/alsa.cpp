#include "alsa.h"
#include "errors.h"

namespace NSend {

TSend::TSend(
    std::uint16_t bitsPerSample,
    std::uint8_t channels,
    std::uint32_t rate,
    std::string device
)
: BitsPerSample(std::move(bitsPerSample))
, Channels(channels)
, Rate(rate) 
, Device(std::move(device)){
}

TSend::TSend(TSend&& alsa) noexcept {
    std::swap(BitsPerSample, alsa.BitsPerSample);
    std::swap(Channels, alsa.Channels);
    std::swap(Rate, alsa.Rate);
    std::swap(Device, alsa.Device);
    std::swap(SoundDevice, alsa.SoundDevice);
    std::swap(HwParams, alsa.HwParams);
    std::swap(FrameSize, alsa.FrameSize);
}

TSend::~TSend() {
    if (SoundDevice != nullptr) {
        snd_pcm_drain(SoundDevice);
        snd_pcm_close(SoundDevice);
    }
}

std::error_code TSend::Init() noexcept {
    if (auto err = snd_pcm_open(&SoundDevice, Device.c_str(), SND_PCM_STREAM_PLAYBACK, 0); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    snd_pcm_hw_params_alloca(&HwParams);
 
    if (auto err = snd_pcm_hw_params_any(SoundDevice, HwParams); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params_set_rate_resample(SoundDevice, HwParams, 1); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params_set_access(SoundDevice, HwParams, SND_PCM_ACCESS_RW_INTERLEAVED); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (Channels > 2) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params_set_channels(SoundDevice, HwParams, Channels); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    snd_pcm_format_t format;
    if (BitsPerSample == 24) {
        format = SND_PCM_FORMAT_S24_3LE;
        FrameSize = 3 * Channels;
    } else {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params_set_format(SoundDevice, HwParams, format); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (Rate != 48000) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params_set_rate_near(SoundDevice, HwParams, &Rate, 0); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    snd_pcm_uframes_t bufferSize = Rate * FrameSize;
    if (auto err = snd_pcm_hw_params_set_buffer_size_near(SoundDevice, HwParams, &bufferSize); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params(SoundDevice, HwParams); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_prepare(SoundDevice); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    return {};
}

std::error_code TSend::Send(TData&& data) noexcept {
    if (SoundDevice == nullptr) {
        return make_error_code(EErrorCode::DeviceInit);
    }
    auto frames = data.size() / FrameSize;

    auto err = snd_pcm_writei(SoundDevice, data.data(), frames);

    if (err == -ENODEV) {
        return Init();
    } else if (err == -EPIPE) {
        snd_pcm_prepare(SoundDevice);
        snd_pcm_writei(SoundDevice, data.data(), frames);
    }

    return {};
}

}
