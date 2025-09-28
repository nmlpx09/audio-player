#include "errors.h"
#include "wav.h"

#include <fcntl.h>
#include <unistd.h>

namespace NRead {

TWav::~TWav() {
    close(Fd);
}

std::expected<TSampleFormat, std::error_code> TWav::Init(std::string fileName, std::size_t delay) noexcept {
    Fd = open(fileName.c_str(), O_RDONLY);

    if (Fd < 0) {
        return std::unexpected(EErrorCode::FileOpen);
    }

    TWavHeader wavHeader;

    read(Fd, reinterpret_cast<std::uint8_t*>(&wavHeader), sizeof(TWavHeader));

    if (std::string{wavHeader.Format, 4} != "WAVE") {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TSampleFormat::NumChannelsPermited.contains(wavHeader.NumChannels)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TSampleFormat::SampleRatePermited.contains(wavHeader.SampleRate)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (!TSampleFormat::BitsPerSamplePermited.contains(wavHeader.BitsPerSample)) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    DataSize = wavHeader.NumChannels * wavHeader.SampleRate * (wavHeader.BitsPerSample / 8) * delay / 1000;

    return TSampleFormat {
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
