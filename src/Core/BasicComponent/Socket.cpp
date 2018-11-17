#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Socket::Socket() :
        SocketFD(-1),
        Address({0}) {
}

Socket::Socket(struct SocketAddress &SocketAddress) :
        SocketFD(-1),
        Address(SocketAddress) {
}

Socket::Socket(int SocketFD, struct SocketAddress &SocketAddress) :
        SocketFD(SocketFD),
        Address(SocketAddress) {
    Active = (SocketFD == -1 ? 0 : 1);
}

int Socket::GetSocketFD() {
    return SocketFD;
}
