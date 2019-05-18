#include "Core/Core.h"

using namespace ngx::Core::Support;

TCPListening::TCPListening() : Listen() {
    FD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
};


SocketError TCPListening::bind(const Address_t &Addr) {
    if (Address.Addr4.sa_family != AF_UNSPEC) {
        return {EALREADY, "TCPListen is already bind"};
    } else if (Addr.Addr4.sa_family == AF_UNSPEC) {
        return {EBADF, "TCPListen bind() got bad input address"};
    } else if (FD == -1) {
        return {EFAULT, "TCPListen can not create a socket"};
    } else if (-1 == ::bind(FD, &Address.Addr4, sizeof(Address_t))) {
        return {errno, "TCPListen failed when bind() socket"};
    }
    Address = Addr;
    return {0};
}
