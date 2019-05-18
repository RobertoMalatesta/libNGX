#include "Core/Core.h"

using namespace ngx::Core::Support;

Listen::Listen() : FD(-1), Address() {}

Listen::Listen(int FD, Address_t &Addr) : FD(FD), Address(Addr) {};

Listen::~Listen() { close(); }

SocketError Listen::setPortReuse(unsigned On) {
    int Code, Val = On ? 1 : 0;

    Code = setsockopt(FD, SOL_SOCKET, SO_REUSEPORT, &Val, sizeof(int));
    return {Code, Code == 0 ? "" : "setPortReuse() failed, setsockopt() failed"};
}

SocketError Listen::listen(int Backlog) {
    if (FD == -1)
        return {EINVAL, "bad socket!"};
    else if (-1 == ::listen(FD, Backlog))
        return {errno, "listen to Socket failed!"};
    return {0};
}

SocketError Listen::close() {
    if (FD != -1)
        ::close(FD), FD = -1;
    Address.Addr4.sa_family = AF_UNSPEC;
    return {0};
}
