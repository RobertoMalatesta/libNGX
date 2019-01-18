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
