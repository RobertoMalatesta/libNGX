#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Listening::Listening() : Socket() {}

Listening::Listening(SocketAddress &Address) :
        Socket(Address) {}

Listening::Listening(int SocketFD, SocketAddress &Address) :
        Socket(SocketFD, Address) {
}

SocketError Listening::SetPortReuse(bool On) {

    int Val = On ? 1 : 0;

    if (setsockopt(SocketFD, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int))) {
        return {errno, "setsockopt() failed"};
    }

    return {0};
}

SocketError Listening::Listen() {

    if (SocketFD == -1) {
        return {EINVAL, "bad socket!"};
    }

    if (-1 == listen(SocketFD, Backlog)) {
        return {errno, "listen to Socket failed!"};
    }

    return {0};
}
