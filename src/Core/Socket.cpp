#include "Core/Core.h"

using namespace ngx::Core;

Socket::Socket() {
    this->SocketFd = -1;
    this->SocketAddress = {0};
}

Socket::Socket(struct SocketAddress &SocketAddress) :
        SocketAddress(SocketAddress) {
    this->SocketFd = -1;
}

Socket::Socket(int SocketFd, struct SocketAddress &SocketAddress) :
        SocketFd(SocketFd),
        SocketAddress(SocketAddress) {

    this->SocketAddress = SocketAddress;
    this->SocketFd = SocketFd;

    Active = (SocketFd == -1 ? 0 : 1);
}

int Socket::GetSocketFD() {
    return SocketFd;
}
