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
    Type = SOCK_TYPE_STREAM, Version = 4;
}

TCP4Listening::~TCP4Listening() {
    Close();
}

SocketError TCP4Listening::Bind() {

    int Code =0;

    if (SocketFD == -1 || Active == 0) {

        SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (SocketFD == -1) {
            return {errno, "failed to create socket"};
        }

        Code = bind(SocketFD, &Address.sockaddr, Address.SocketLength);

        if (Code == -1) {
            return {errno, "failed to bind socket"};
        }

        Active = 1;
    }

    return {0};
}

SocketError TCP4Listening::Listen() {

    if (SocketFD == -1) {
        return {EINVAL, "bad Socket!"};
    }

    if (-1 == listen(SocketFD, Backlog)) {
        return {errno, "listen to Socket failed!"};
    }

    return {0};
}

SocketError TCP4Listening::Close() {

    if (SocketFD != -1 || Active == 1) {
        close(SocketFD);
        SocketFD = -1, Active = 0;
    }

    return {0};
}

SocketError TCP4Listening::SetPortReuse(bool Open) {

    int Val = Open ? 1 : 0;

    if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int))) {
        return {errno, "setsockopt() failed"};
    }

    return {0};
}
