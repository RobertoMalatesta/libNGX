#include "Core/Core.h"

using namespace ngx::Core;

Listening::Listening() : Socket() {
    IsListen = 1;
}

Listening::Listening(Core::SocketAddress &SocketAddress) :
        Socket(SocketAddress) {
    IsListen = 1;
}

Listening::Listening(int SocketFd, Core::SocketAddress &SocketAddress) :
        Socket(SocketFd, SocketAddress) {
    IsListen = 1;
}

TCP4Listening::TCP4Listening(Core::SocketAddress &SocketAddress)
        : Listening(SocketAddress) {
    SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    Type = SOCK_TYPE_STREAM;
    Version = 4;

    if (0 == bind(SocketFd, &SocketAddress.sockaddr, SocketAddress.SocketLength)) {
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

    int Val = Open ? 1 : 0;

    if ((Open && Reuse) || (!Open && !Reuse)) {
        return SocketError(EALREADY);
    }

    if (setsockopt(SocketFd, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int))) {
        return SocketError(errno);
    }
    return SocketError(0);
}
