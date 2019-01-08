#include "Core/Core.h"
#include <netinet/tcp.h>

using namespace ngx::Core::BasicComponent;

Socket::Socket() :
        SocketFD(-1),
        Address() {
}

Socket::Socket(SocketAddress &Address) :
        SocketFD(-1),
        Address(Address) {
}

Socket::Socket(int SocketFD, SocketAddress &Address) :
        SocketFD(SocketFD),
        Address(Address) {
}

SocketError Socket::SetSocketAddress(int SocketFD, SocketAddress &Address) {

    LockGuard Guard(&SocketLock);

    this->SocketFD = SocketFD;
    this->Address = Address;

    return {0};
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

    return {Code};
}

SocketError Socket::SetNoDelay(bool On) {

    int NoDelay = On? 1: 0, Code = 0;

    Code = setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &NoDelay, sizeof(NoDelay));

    return {Code, Code == 0 ? "" :"setsockopt() failed"};
}

SocketError Socket::Close() {

    if (SocketFD != -1) {
        close(SocketFD), SocketFD = -1;
    }

    return {0};
}
