#include <netinet/tcp.h>
#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(size_t BufferBlockSize, uint64_t BufferRecycleBinSize,
                                             uint64_t ConnectionRecycleBinSize) :
        BackendRecycleBin(ConnectionRecycleBinSize),
        BB(BufferBlockSize, BufferRecycleBinSize),
        TCPNoDelay(1), TCPNoPush(0) {

}

int HTTPConnectionBuilder::Get(HTTPConnection *&C, int SocketFD, SocketAddress &SocketAddress, HTTPServer *Server,
                               SocketEventDomain *EventDomain) {

    LockGuard LockGuard(&Lock);

    if (SocketFD == -1 || BackendRecycleBin.Get(C, SocketFD, SocketAddress) != 0) {
        return C = nullptr, -1;
    }

    TCPNoDelay = 1;

    setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &TCPNoDelay, sizeof(TCPNoDelay));

    // configure connection
    C->Open = 1;
    C->ParentServer = Server;
    C->ParentEventDomain = EventDomain;
    C->TimerNode.Reset();
    C->Lock.Unlock();

    BB.BuildBuffer(C->ReadBuffer);

    return 0;
}

int HTTPConnectionBuilder::Put(HTTPConnection *&C) {
    LockGuard LockGuard(&Lock);
    return BackendRecycleBin.Put(C);
}