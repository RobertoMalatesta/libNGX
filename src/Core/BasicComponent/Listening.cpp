#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Listening::Listening() : Socket() {
    IsListen = 1;
}

Listening::Listening(SocketAddress &SocketAddress) :
        Socket(SocketAddress) {
    IsListen = 1;
}

Listening::Listening(int SocketFD, SocketAddress &SocketAddress) :
        Socket(SocketFD, SocketAddress) {
    IsListen = 1;
}

TCP4Listening::TCP4Listening(SocketAddress &SocketAddress)
        : Listening(SocketAddress) {
    SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    Type = SOCK_TYPE_STREAM;
    Version = 4;

    if (0 == bind(SocketFD, &Address.sockaddr, Address.SocketLength)) {
        Active = 1;
    }
}

TCP4Listening::~TCP4Listening() {
    if (SocketFD != -1) {
        close(SocketFD);
        Active = 0;
        SocketFD = -1;
    }
}

SocketError TCP4Listening::Listen() {

    if (SocketFD == -1) {
        return {EINVAL, "Bad Socket!"};
    }

    if (-1 == listen(SocketFD, Backlog)) {
        return {errno, "Listen to Socket failed!"};
    }
    return {0};
}

SocketError TCP4Listening::Close() {

    if (SocketFD != -1) {
        close(SocketFD);
        SocketFD = -1;
    }

    return {0};
}

SocketError TCP4Listening::SetPortReuse(bool Open) {

    int Val = Open ? 1 : 0;

    if ((Open && Reuse) || (!Open && !Reuse)) {
        return {EALREADY};
    }

    if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int))) {
        return {errno};
    }
    return {0};
}
