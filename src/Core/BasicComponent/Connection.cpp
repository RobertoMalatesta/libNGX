#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::BasicComponent;

Connection::Connection() : Socket() {
}

Connection::Connection(struct SocketAddress &SocketAddress) :
        Socket(SocketAddress) {
}

Connection::Connection(int SocketFD, struct SocketAddress &SocketAddress)
        : Socket(SocketFD, SocketAddress) {
};

TCP4Connection::TCP4Connection(int SocketFD, struct SocketAddress &SocketAddress) :
        Connection(SocketFD, SocketAddress) {
    Type = SOCK_TYPE_STREAM, Version = 4;
}

TCP4Connection::TCP4Connection(struct SocketAddress &SocketAddress) :
        Connection(SocketAddress) {
    Type = SOCK_TYPE_STREAM, Version = 4;
}

SocketError TCP4Connection::Connect() {

    if (SocketFD == -1) {
        return {EFAULT, "Invalid Socket"};
    }

    if (Active == 1) {
        return {EALREADY, "Socket is already active!"};
    }

    SocketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Type = SOCK_TYPE_STREAM;

    if (0 == bind(SocketFD, &Address.sockaddr, Address.SocketLength)) {
        Active = 1;
    }

    return {0};
}

SocketError TCP4Connection::Close() {

    // Close the connection if open
    if (SocketFD != -1 || Open == 1) {
        close(SocketFD);
        SocketFD = -1, Open = 0;
    }

    return {0};
}
