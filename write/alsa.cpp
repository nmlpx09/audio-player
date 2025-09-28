#include "alsa.h"
#include "errors.h"

namespace NWrite {

TWrite::TWrite(std::string device) : Device(std::move(device)) {
}

TWrite::TWrite(TWrite&& alsa) noexcept {
    std::swap(Device, alsa.Device);
    std::swap(SoundDevice, alsa.SoundDevice);
    std::swap(HwParams, alsa.HwParams);
    std::swap(FrameSize, alsa.FrameSize);
}

TWrite::~TWrite() {
    if (SoundDevice != nullptr) {
        snd_pcm_drain(SoundDevice);
        snd_pcm_close(SoundDevice);
    }
}

std::error_code TWrite::Init(TFormat sampleFormat) noexcept {
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

    if (!TFormat::NumChannelsPermited.contains(sampleFormat.NumChannels)) {
        return EErrorCode::DeviceInit;
    }

    if (auto err = snd_pcm_hw_params_set_channels(SoundDevice, HwParams, sampleFormat.NumChannels); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (!TFormat::BitsPerSamplePermited.contains(sampleFormat.BitsPerSample)) {
        return EErrorCode::DeviceInit;
    }

    snd_pcm_format_t format;
    if (sampleFormat.BitsPerSample == 24) {
        format = SND_PCM_FORMAT_S24_3LE;
        FrameSize = 3 * sampleFormat.NumChannels;
    } else if (sampleFormat.BitsPerSample == 16) {
        format = SND_PCM_FORMAT_S16_LE;
        FrameSize = 2 * sampleFormat.NumChannels;
    } else if (sampleFormat.BitsPerSample == 32) {
        format = SND_PCM_FORMAT_S32_LE;
        FrameSize = 4 * sampleFormat.NumChannels;
    } else {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (auto err = snd_pcm_hw_params_set_format(SoundDevice, HwParams, format); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    if (!TFormat::SampleRatePermited.contains(sampleFormat.SampleRate)) {
        return EErrorCode::DeviceInit;
    }

    if (auto err = snd_pcm_hw_params_set_rate_near(SoundDevice, HwParams, &sampleFormat.SampleRate, 0); err < 0) {
        return make_error_code(EErrorCode::DeviceInit);
    }

    snd_pcm_uframes_t bufferSize = sampleFormat.SampleRate * FrameSize;
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

std::error_code TWrite::Write(const TCallback& callback) noexcept {
    TFormat currentFormat;

    while (true) {
        if (auto data = callback(); !data) {
            break;
        } else {
            auto&& [format, buffer] = data.value();
            if (currentFormat != format) {
                if(auto ec = Init(format); ec) {
                    return ec;
                } else {
                    currentFormat = format;
                }
            }

            auto frames = buffer.size() / FrameSize;

            auto err = snd_pcm_writei(SoundDevice, buffer.data(), frames);

            if (err == -ENODEV) {
                if (auto ec = Init(format); ec) {
                    return ec;
                }
            } else if (err == -EPIPE) {
                snd_pcm_prepare(SoundDevice);
                snd_pcm_writei(SoundDevice, buffer.data(), frames);
            }
        }
    }

    return {};
}

}
