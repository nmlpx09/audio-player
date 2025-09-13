#pragma once

#include "interface.h"

#include <netinet/in.h>
#include <sys/socket.h>

namespace NSend {

struct TSocket: TInterface {
public:
    TSocket(std::string ip, std::uint16_t port);
    TSocket(const TSocket&) = delete;
    TSocket(TSocket&&) noexcept = default;
    ~TSocket();
    TSocket& operator=(const TSocket&) = delete;
    TSocket& operator=(TSocket&&) = delete;

    std::error_code Init() noexcept override;
    std::error_code Send(TData&& data) noexcept override;

private:
    std::string Ip;
    std::uint16_t Port = 0;
    std::int32_t Sockfd = -1;
    sockaddr_in Sockaddr;
};

}
