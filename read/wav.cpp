#include "errors.h"
#include "wav.h"

#include <fcntl.h>
#include <unistd.h>

namespace NRead {

TWav::~TWav() {
    close(Fd);
}

std::expected<TFormat, std::error_code> TWav::Init(std::string fileName, std::size_t delay) noexcept {
    Fd = open(fileName.c_str(), O_RDONLY);

    if (Fd < 0) {
        return std::unexpected(EErrorCode::FileOpen);
    }

    TWavHeader wavHeader;

    read(Fd, reinterpret_cast<std::uint8_t*>(&wavHeader), sizeof(TWavHeader));

    if (std::string{wavHeader.Format, 4} != "WAVE") {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TFormat::NumChannelsPermited.contains(wavHeader.NumChannels)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TFormat::SampleRatePermited.contains(wavHeader.SampleRate)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TFormat::BitsPerSamplePermited.contains(wavHeader.BitsPerSample)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    DataSize = wavHeader.NumChannels * wavHeader.SampleRate * (wavHeader.BitsPerSample / 8) * delay / 1000;

    return TFormat {
        .BitsPerSample = wavHeader.BitsPerSample,
        .NumChannels = wavHeader.NumChannels,
        .SampleRate = wavHeader.SampleRate
    };
}

std::error_code TWav::Read(const TCallback& callback) noexcept {
    while (true) {
        TData buffer(DataSize, 0);

        auto size = read(Fd, buffer.data(), DataSize);

        if (size >= 0 && size < DataSize) {
            return {};
        } else if (size < 0) {
            return EErrorCode::FileOpen;
        }

        callback(std::move(buffer));
    }

    return {};
}

}
