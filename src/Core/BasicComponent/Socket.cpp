#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Socket::Socket():
    SocketFd(-1),
    SocketAddress({0}) {
}

Socket::Socket(struct SocketAddress &SocketAddress) :
        SocketFd(-1),
        SocketAddress(SocketAddress) {
}

Socket::Socket(int SocketFd, struct SocketAddress &SocketAddress) :
        SocketFd(SocketFd),
        SocketAddress(SocketAddress) {
    Active = (SocketFd == -1 ? 0 : 1);
}

int Socket::GetSocketFD() {
    return SocketFd;
}
