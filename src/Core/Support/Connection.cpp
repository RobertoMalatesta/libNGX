#include "Core/Core.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/types.h>

using namespace ngx::Core::Support;

SocketError Connection::close() {
    if (FD != -1)
        ::close(FD), FD = -1;
    Address.Addr4.sa_family = AF_UNSPEC;
    return {0};
}

SocketError Connection::setNoBlock(unsigned On) {

    int flags = fcntl(FD, F_GETFL), Code;

    if (flags == -1)
        return {errno, "failed to setNoBlock(), failed to get non-blocking in fnctl()"};

    Code = fcntl(FD, F_SETFL, (On ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)));
    return {Code, Code == 0 ? "" : "failed to setNoBlock(), failed to set non-blocking in fnctl()"};
}

SocketError Connection::setNoDelay(unsigned On) {

    int Code, NoDelay = On ? 1 : 0;

    Code = setsockopt(FD, IPPROTO_TCP, TCP_NODELAY, (void *) &NoDelay, sizeof(NoDelay));

    return {Code, Code == 0 ? "" : "failed to setNoDelay(), failed to set no-delay in setsockopt()"};
}
