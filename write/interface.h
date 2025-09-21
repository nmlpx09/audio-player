#pragma once

#include <common/types.h>

#include <memory>
#include <system_error>

namespace NWrite {

struct TInterface {
public:
    virtual std::error_code Init() noexcept = 0;
    virtual std::error_code Write(TData&& data) noexcept = 0;
    virtual ~TInterface() {};
};

using TWritePtr = std::unique_ptr<TInterface>;

}