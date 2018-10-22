#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core;

Connection::Connection() : Socket() {
}

Connection::Connection(struct SocketAddress &SocketAddress) :
        Socket(SocketAddress) {}

Connection::Connection(int SocketFd, struct SocketAddress &SocketAddress)
        : Socket(SocketFd, SocketAddress) {};

TCP4Connection::TCP4Connection(int SocketFd, struct SocketAddress &SocketAddress) :
        Connection(SocketFd, SocketAddress) {}

TCP4Connection::TCP4Connection(struct SocketAddress &SocketAddress) :
        Connection(SocketAddress) {
    SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Type = SOCK_TYPE_STREAM;

    if (0 == bind(SocketFd, &SocketAddress.sockaddr, SocketAddress.SocketLength)) {
        Active = 1;
    }
}

SocketError TCP4Connection::Connect() {
    if (SocketFd == -1) {
        return SocketError(EFAULT, "Invalid Socket");
    }

    if (Active == 1) {
        return SocketError(EALREADY, "Socket is already active!");
    }

    return SocketError(0);
}

SocketError TCP4Connection::Close() {
    if (SocketFd != -1) {
        close(SocketFd);
        SocketFd = -1;
        Active = 0;
    }
    return SocketError(0);
}

SocketError TCP4Connection::SetNoDelay(bool Open) {

    unsigned int TCPNoDelay = Open ? 1 : 0;

    if ((Nodelay == 1 && TCPNoDelay == 1) || (Nodelay == 0 && TCPNoDelay == 0)) {
        return SocketError(0, "No delay is already set!");
    }

    if (setsockopt(SocketFd, IPPROTO_TCP, TCP_NODELAY,
                   (const void *) &TCPNoDelay, sizeof(int)) == -1) {
        return SocketError(errno, "setsockopt() failed to set TCP_NODELAY!");
    }

    Nodelay = TCPNoDelay;
    return SocketError(0);
}