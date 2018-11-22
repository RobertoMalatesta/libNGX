#include "Core/Core.h"
#include <netinet/tcp.h>

using namespace ngx::Core::BasicComponent;

Socket::Socket() :
        SocketFD(-1),
        Address({0}) {
}

Socket::Socket(struct SocketAddress &SocketAddress) :
        SocketFD(-1),
        Address(SocketAddress) {
}

Socket::Socket(int SocketFD, struct SocketAddress &SocketAddress) :
        SocketFD(SocketFD),
        Address(SocketAddress) {
    Active = (SocketFD == -1 ? 0 : 1);
}

SocketError Socket::SetNonBlock(bool On) {

    int flags = fcntl(SocketFD, F_GETFL), Code = 0;

    if (flags == -1) {
        return errno;
    } else if (On) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    Code = fcntl(SocketFD, F_SETFL, flags | O_NONBLOCK);

    return {Code, "fcntl() failed"};
}

SocketError Socket::SetNoDelay(bool On) {

    int NoDelay = On? 1: 0, Code = 0;

    Code = setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &NoDelay, sizeof(NoDelay));

    return {Code, "setsockopt() failed"};
}