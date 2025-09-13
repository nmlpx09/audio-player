#include "rtp.h"

#include <algorithm>
#include <cstring>
#include <iterator>

namespace NRtp {

TRtp::TRtp(TData data) {    
    Data.reserve(data.size() + sizeof(TRtpHeaders));

    auto headers = TRtpHeaders{};

    auto* pheaders = reinterpret_cast<TData::value_type*>(&headers);
    for (std::size_t index = 0; index < sizeof(TRtpHeaders); ++index) {
        Data.push_back(pheaders[index]);
    }

    std::copy(data.begin(), data.end(), std::inserter(Data, Data.begin() + sizeof(TRtpHeaders)));
}

TData::const_pointer TRtp::GetData() const noexcept {
    return Data.data();
}

std::size_t TRtp::Size() const noexcept {
    return Data.size();
}

}
