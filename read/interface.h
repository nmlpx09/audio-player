#pragma once

#include <common/types.h>

#include <cstdint>
#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <system_error>

namespace NRead {

using TCallback = std::function<void(TData)>;

struct TInterface {
public:
    virtual std::expected<TSampleFormat, std::error_code> Init(std::string, std::size_t) noexcept = 0;
    virtual std::error_code Read(const TCallback&) noexcept = 0;
    virtual ~TInterface() {};
};

using TReadPtr = std::unique_ptr<TInterface>;

}
