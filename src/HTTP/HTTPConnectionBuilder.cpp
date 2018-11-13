#include <netinet/tcp.h>
#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(size_t BufferBlockSize, uint64_t BufferRecyclerSize,
                                             uint64_t ConnectionRecyclerSize) :
        BackendRecycler(ConnectionRecyclerSize),
        BB(BufferBlockSize, BufferRecyclerSize),
        TCPNoDelay(1),
        TCPNoPush(0) {
}

HTTPConnection *HTTPConnectionBuilder::Get(int SocketFD, SocketAddress *SocketAddress, HTTPServer *Server,
                                           Listening *Listening, SocketEventDomain *EventDomain) {

    SpinlockGuard LockGuard(&Lock);

    if (SocketFD == -1) {
        return nullptr;
    }

    HTTPConnection *Connection = BackendRecycler.Get(SocketFD, *SocketAddress);

    if (Connection == nullptr) {
        return nullptr;
    }

    setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY, (void *) &TCPNoDelay, sizeof(TCPNoDelay));

    // configure connection
    Connection->Closed = false;
    Connection->ParentServer = Server;
    Connection->ParentListeing = Listening;
    Connection->ParentEventDomain = EventDomain;

    Connection->TimerNode.Reset();
    BB.BuildBuffer(Connection->ReadBuffer);

    return Connection;
}

void HTTPConnectionBuilder::Put(HTTPConnection *C) {
    SpinlockGuard LockGuard(&Lock);
    if (C != nullptr) {
        BackendRecycler.Put(C);
    }
}