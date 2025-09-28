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

    if (wavHeader.NumChannels != 2) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (wavHeader.SampleRate != 48000 && wavHeader.SampleRate != 44100) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    if (wavHeader.BitsPerSample != 24 && wavHeader.BitsPerSample != 16) {
        return std::unexpected(EErrorCode::FileFormat);
    }

    std::uint16_t bytesPerSample = 0;
    if (wavHeader.BitsPerSample == 24) {
        bytesPerSample = 3;
    } else if (wavHeader.BitsPerSample == 16) {
        bytesPerSample = 2;
    }

    DataSize = wavHeader.NumChannels * wavHeader.SampleRate * bytesPerSample * delay / 1000;

    return TSampleFormat {
        .BytesPerSample = bytesPerSample,
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
