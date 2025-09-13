#pragma once

#include <system_error>

namespace NSend {

enum EErrorCode {
    Ok,
    SocketOpen,
    SocketInit,
    DeviceInit,
    Channels,
    Format,
    Rate,
    Prepare,
    Write
};

}

namespace std {

template<> struct is_error_code_enum<NSend::EErrorCode> : std::true_type{};

}

namespace NSend {

class TErrorCategory final : public std::error_category {
public:
    const char* name() const noexcept override {
        return "send error";
    }

    std::string message(int value) const override {
        switch (value) {
            case Ok:
                return "ok";
            case SocketOpen:
                return "socket open error";
            case SocketInit:
                return "socket init error";
            case DeviceInit:
                return "device init error";
            case Channels:
                return "channels not supported error";
            case Format:
                return "format not supported error";
            case Rate:
                return "rate not supported error";
            case Prepare:
                return "prepare device error";
            case Write:
                return "write device error";
        }

        return "unknown error code: " + std::to_string(value);
    }

    static const TErrorCategory& instance() {
        static const TErrorCategory errorCategory;
        return errorCategory;
    }
};

inline std::error_code make_error_code(EErrorCode errorCode) {
    return {errorCode, TErrorCategory::instance()};
}

}
