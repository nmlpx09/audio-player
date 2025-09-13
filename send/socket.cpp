#include "errors.h"
#include "socket.h"

#include <rtp/rtp.h>

#include <arpa/inet.h>
#include <unistd.h>

namespace NSend {

TSocket::TSocket(std::string ip, std::uint16_t port)
: Ip(std::move(ip))
, Port(port) {
}

TSocket::~TSocket() {
    close(Sockfd);
}

std::error_code TSocket::Init() noexcept {
    Sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (Sockfd < 0) {
        return EErrorCode::SocketOpen;
    }

    Sockaddr = sockaddr_in {
        .sin_family = AF_INET,
        .sin_port = htons(Port),
        .sin_addr = {
            .s_addr = inet_addr(Ip.c_str())
        },
        .sin_zero = {0}
    };

    return {};
}

std::error_code TSocket::Send(TData&& data) noexcept {
    if (Sockfd < 0) {
        return EErrorCode::SocketInit;
    }

    auto rtp = NRtp::TRtp{std::move(data)};

    sendto(Sockfd, rtp.GetData(), rtp.Size(), 0, (const sockaddr*)&Sockaddr, sizeof(sockaddr));

    return {};
}

}
