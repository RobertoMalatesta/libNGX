#include <netinet/tcp.h>
#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(size_t BufferBlockSize, uint64_t BufferRecycleBinSize,
                                             uint64_t ConnectionRecycleBinSize) :
        BackendRecycleBin(ConnectionRecycleBinSize),
        BB(BufferBlockSize, BufferRecycleBinSize),
        TCPNoDelay(1), TCPNoPush(0) {}

int HTTPConnectionBuilder::Get(HTTPConnection *&C, int SocketFD, SocketAddress *SocketAddress, HTTPServer *Server,
                               Listening *Listening, SocketEventDomain *EventDomain) {

    SpinlockGuard LockGuard(&Lock);

    if (SocketFD == -1 || BackendRecycleBin.Get(C, SocketFD, *SocketAddress) != 0) {
        return C = nullptr, -1;
    }

    setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &TCPNoDelay, sizeof(TCPNoDelay));

    // configure connection
    C->Open = 1;
    C->Lock.Unlock();
    C->ParentServer = Server;
    C->ParentListeing = Listening;
    C->ParentEventDomain = EventDomain;
    C->TimerNode.Reset();

    BB.BuildBuffer(C->ReadBuffer);

    return 0;
}

int HTTPConnectionBuilder::Put(HTTPConnection *&C) {
    SpinlockGuard LockGuard(&Lock);
    return BackendRecycleBin.Put(C);
}