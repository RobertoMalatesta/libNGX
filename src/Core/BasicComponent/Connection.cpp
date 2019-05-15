#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::BasicComponent;

Connection::Connection() : Socket() {
}

Connection::Connection(Address_t &Address) :
        Socket(Address) {
}

Connection::Connection(int SocketFD, Address_t &Address)
        : Socket(SocketFD, Address) {
};
