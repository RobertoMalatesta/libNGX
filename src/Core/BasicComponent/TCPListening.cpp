#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

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