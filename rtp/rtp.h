#pragma once

#include <common/types.h>

#include <cstdint>

namespace NRtp {

struct TRtpHeaders {
    std::uint32_t cc : 4  = 0;
    std::uint32_t x  : 1  = 0;
    std::uint32_t p  : 1  = 0;
    std::uint32_t v  : 2  = 0;
    std::uint32_t pt : 7  = 0;
    std::uint32_t m  : 1  = 0;
    std::uint32_t sn : 16 = 0;
    std::uint32_t ts      = 0;
    std::uint32_t ssrc    = 0; 
};

struct TRtp {
public:
    TRtp() = delete;
    TRtp(TData buffer);
    TRtp(const TRtp&) = delete;
    TRtp(TRtp&&) noexcept = default;
    TRtp& operator=(const TRtp&) = delete;
    TRtp& operator=(TRtp&&) = delete;

    TData::const_pointer  GetData() const noexcept;
    std::size_t Size() const noexcept;

private:
    TData Data;
};

}
