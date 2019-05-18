#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::Support;

TCPConnection::TCPConnection() : Connection() {
    FD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

SocketError TCPConnection::connect(Address_t &Addr) {
    if (Address.Addr4.sa_family != AF_UNSPEC) {
        return {EALREADY, "TCPConnection is already connected"};
    } else if (Addr.Addr4.sa_family == AF_UNSPEC) {
        return {EBADF, "TCPConnection connect() got bad input address"};
    } else if (FD == -1) {
        return {EFAULT, "TCPConnection can not create a socket"};
    } else if (::connect(FD, &Address.Addr4, sizeof(Address)) != 0) {
        return {errno, "TCPConnection connect() failed, ::connect() failure"};
    }
    Address = Addr;
    return {0};
}
