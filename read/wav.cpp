#include "errors.h"
#include "wav.h"

#include <fcntl.h>
#include <unistd.h>

namespace NRead {

TWav::TWav(
    std::string fileName,
    std::uint16_t bitsPerSample,
    std::uint8_t cannels,
    std::uint32_t rate,
    std::int32_t dataSize
)
: FileName(std::move(fileName))
, BitsPerSample(std::move(bitsPerSample))
, Cannels(cannels)
, Rate(rate)
, DataSize(dataSize) {
}

TWav::~TWav() {
    close(Fd);
}

std::error_code TWav::Init() noexcept {
    Fd = open(FileName.c_str(), O_RDONLY);

    if (Fd < 0) {
        return EErrorCode::FileOpen;
    }

    TWavHeader wavHeader;

    read(Fd, reinterpret_cast<std::uint8_t*>(&wavHeader), sizeof(TWavHeader));

    if (std::string{wavHeader.Format, 4} != "WAVE") {
        return EErrorCode::FileFormat;
    }

    if (wavHeader.NumChannels != Cannels) {
        return EErrorCode::FileFormat;
    }

    if (wavHeader.SampleRate != Rate) {
        return EErrorCode::FileFormat;
    }

    if (wavHeader.BitsPerSample != BitsPerSample) {
        return EErrorCode::FileFormat;
    }

    return {};
}

std::expected<TData, std::error_code> TWav::Rcv() const noexcept {
    TData buffer(DataSize, 0);
    
    auto size = read(Fd, buffer.data(), DataSize);
    
    if (size >= 0 && size < DataSize) {
        return std::unexpected(EErrorCode::FileEnd);
    } else if (size < 0) {
        return std::unexpected(EErrorCode::FileOpen);
    }
    return buffer;
}

}
