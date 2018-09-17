#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core;

Socket::Socket(int SocketFd) {
    this->SocketFd = SocketFd;
}

Socket::Socket(struct sockaddr *SocketAddress, socklen_t SocketLength) {
    this->SocketFd = -1;
    this->SocketAddress = SocketAddress;
    this->SocketLength = SocketLength;
}

int Socket::GetSocketFD() {
    return SocketFd;
}

Listening::Listening(struct sockaddr *SocketAddress, socklen_t SocketLength):
        Socket(SocketAddress, SocketLength) {
    IsListen = 1;
}

Listening::Listening(int SocketFd) : Socket(SocketFd) {
    IsListen = 1;
}

TCP4Listening::TCP4Listening(struct sockaddr *SocketAddress, socklen_t SocketLength)
        : Listening(SocketAddress, SocketLength) {
    SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Type = SOCK_TYPE_STREAM;

    if ( 0 == bind(SocketFd, SocketAddress, SocketLength)) {
        Active = 1;
    }
}

TCP4Listening::~TCP4Listening() {
    if (SocketFd != -1) {
        close(SocketFd);
        Active = 0;
        SocketFd = -1;
    }
}

SocketError TCP4Listening::Listen() {

    if (SocketFd == -1) {
        return SocketError(EINVAL, "Bad Socket!");
    }

    if (-1 == listen(SocketFd, Backlog)) {
        return SocketError(errno, "Listen to Socket failed!");
    }
    return SocketError(0);
}

SocketError TCP4Listening::SetPortReuse(bool Open) {

    int Val = Open? 1: 0;

    if ((Open && Reuse) || (!Open && !Reuse)) {
        return SocketError(EALREADY);
    }

    if (setsockopt(SocketFd, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int))) {
        return SocketError(errno);
    }
    return SocketError(0);
}

Connection::Connection(int SocketFd) :Socket(SocketFd) {};
Connection::Connection(struct sockaddr *SocketAddress, socklen_t SocketLength):
        Socket(SocketAddress, SocketLength){
}

TCP4Connection::TCP4Connection(int SocketFd): Connection(SocketFd) {
    if (SocketFd != -1) {
        Active = 1;
    }
}

TCP4Connection::TCP4Connection(struct sockaddr *SocketAddress, socklen_t SocketLength):
        Connection(SocketAddress, SocketLength) {
    SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Type = SOCK_TYPE_STREAM;

    if ( 0 == bind(SocketFd, SocketAddress, SocketLength)) {
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

    int  TCPNoDelay = Open? 1: 0;

    if ((Nodelay == 1 && TCPNoDelay == 1) || (Nodelay==0 && TCPNoDelay == 0)) {
        return SocketError(0, "No delay is already set!");
    }

    if (setsockopt(SocketFd, IPPROTO_TCP, TCP_NODELAY,
                   (const void *) &TCPNoDelay, sizeof(int)) == -1) {
        return SocketError(errno, "setsockopt() failed to set TCP_NODELAY!");
    }

    Nodelay = TCPNoDelay;
    return SocketError(0);
}