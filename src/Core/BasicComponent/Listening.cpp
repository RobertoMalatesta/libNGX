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

TCPListening::TCPListening(SocketAddress &Address)
        : Listening(Address) {
}

TCPListening::~TCPListening() {
    Close();
}

SocketError TCPListening::Bind() {

    int Code = 0;

    if (SocketFD == -1) {

        socklen_t SocketLength = sizeof(SocketAddress);

        SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (SocketFD == -1) {
            return {errno, "failed to create socket"};
        }

        Code = bind(SocketFD, &Address.sockaddr, SocketLength);

        if (Code == -1) {

            close(SocketFD), SocketFD = -1;

            return {errno, "failed to bind socket"};
        }
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
