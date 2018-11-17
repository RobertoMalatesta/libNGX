#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::BasicComponent;

Connection::Connection() : Socket() {
}

Connection::Connection(struct SocketAddress &SocketAddress) :
        Socket(SocketAddress) {}

Connection::Connection(int SocketFD, struct SocketAddress &SocketAddress)
        : Socket(SocketFD, SocketAddress) {};

TCP4Connection::TCP4Connection(int SocketFD, struct SocketAddress &SocketAddress) :
        Connection(SocketFD, SocketAddress) {}

TCP4Connection::TCP4Connection(struct SocketAddress &SocketAddress) :
        Connection(SocketAddress) {
    SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Type = SOCK_TYPE_STREAM;

    if (0 == bind(SocketFD, &SocketAddress.sockaddr, SocketAddress.SocketLength)) {
        Active = 1;
    }
}

SocketError TCP4Connection::Connect() {
    if (SocketFD == -1) {
        return {EFAULT, "Invalid Socket"};
    }

    if (Active == 1) {
        return {EALREADY, "Socket is already active!"};
    }

    return {0};
}

SocketError TCP4Connection::Close() {
    if (SocketFD != -1) {
        close(SocketFD);
        SocketFD = -1;
        Active = 0;
    }
    return {0};
}

SocketError TCP4Connection::SetNoDelay(bool Open) {

    unsigned int TCPNoDelay = Open ? 1 : 0;

    if ((Nodelay == 1 && TCPNoDelay == 1) || (Nodelay == 0 && TCPNoDelay == 0)) {
        return {0, "No delay is already set!"};
    }

    if (setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY,
                   (const void *) &TCPNoDelay, sizeof(int)) == -1) {
        return {errno, "setsockopt() failed to set TCP_NODELAY!"};
    }

    Nodelay = TCPNoDelay;

    return {0};
}