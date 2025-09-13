#pragma once

#include <common/types.h>

#include <memory>
#include <system_error>

namespace NSend {

struct TInterface {
public:
    virtual std::error_code Init() noexcept = 0;
    virtual std::error_code Send(TData&& data) noexcept = 0;
    virtual ~TInterface() {};
};

using TSendPtr = std::unique_ptr<TInterface>;

}