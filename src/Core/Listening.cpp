#include "Core/Core.h"

#include <sys/socket.h>
#include <unistd.h>

using namespace ngx::Core;

Listening::Listening(struct sockaddr *SocketAddr) {
    this->SocketAddress = SocketAddr;
}

TCP4Listening::TCP4Listening(struct sockaddr *SockAddr, socklen_t SocketLength) : Listening(SockAddr) {

    Active = 0;
    Stream = 1;
    Version = 4;

    SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SocketAddress = SockAddr;
    this->SocketLength = SocketLength;
}

TCP4Listening::~TCP4Listening() {

    if (SocketFd != -1 && Active == 1) {
        close(SocketFd);
        Active = 0;
        SocketFd = -1;
    }
}

int TCP4Listening::Bind() {

    if (bind(SocketFd, SocketAddress, SocketLength)) {
        switch (errno) {
            case EACCES:
            case EADDRINUSE:
            case EBADF:
            case EINVAL:
            case ENOENT:
            default:
                printf("error binding socket!");
                return -1;
        }
    }

    Active = 1;

    return 0;
}

int TCP4Listening::SetPortReuse(bool Open) {

    int Val = Open? 1: 0;

    if (setsockopt(SocketFd, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int))) {
        printf("setsockopt error!");
        return -1;
    }
    return 0;
}