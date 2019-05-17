#include "Core/Core.h"

using namespace ngx::Core::Support;

Listening::Listening() : Socket() {}

Listening::Listening(Address_t &Address) :
        Socket(Address) {}

Listening::Listening(int SocketFD, Address_t &Address) :
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
