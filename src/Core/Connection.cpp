#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
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

Connection::Connection(int SocketFD) :Socket(SocketFD){
}

SocketError Connection::Connect() {

    if (SocketFd == -1) {
        return SocketError(EFAULT, "Invalid Socket");
    }

    if (Active == 1) {
        return SocketError(EALREADY, "Socket is already active!");
    }

    return SocketError(0);
}

SocketError Connection::Close() {

    if (SocketFd != -1 && Active) {
        close(SocketFd);
        SocketFd = -1;
        Active = 0;
    }

    return SocketError(0);
}

Listening::Listening(struct sockaddr *SocketAddress, socklen_t SocketLength):
        Socket(SocketAddress, SocketLength) {
    SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    IsListen = 1;
}

Listening::Listening(int SocketFd) : Socket(SocketFd) {
    IsListen = 1;
}


TCP4Listening::TCP4Listening(struct sockaddr *SocketAddress, socklen_t SocketLength)
        : Listening(SocketAddress, SocketLength) {
    Type = SOCK_TYPE_STREAM;
}

TCP4Listening::~TCP4Listening() {

    if (SocketFd != -1 && Active == 1) {
        close(SocketFd);
        Active = 0;
        SocketFd = -1;
    }
}

SocketError TCP4Listening::Bind() {

    if (bind(SocketFd, SocketAddress, SocketLength)) {
        return SocketError(errno);
    }

    Active = 1;

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
