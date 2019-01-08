#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::BasicComponent;

Connection::Connection() : Socket() {
}

Connection::Connection(SocketAddress &Address) :
        Socket(Address) {
}

Connection::Connection(int SocketFD, SocketAddress &Address)
        : Socket(SocketFD, Address) {
};

TCPConnection::TCPConnection(int SocketFD, SocketAddress &Address) :
        Connection(SocketFD, Address) {
}

TCPConnection::TCPConnection(SocketAddress &Address) :
        Connection(Address) {
}

SocketError TCPConnection::Connect() {

    if (SocketFD != -1) {
        return {EALREADY, "Socket is already open!"};
    }

    SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (0 != bind(SocketFD, &Address.sockaddr, sizeof(Address))) {
        close(SocketFD), SocketFD = -1;
    }

    return {0};
}
