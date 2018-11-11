#include <netinet/tcp.h>
#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(size_t BufferBlockSize, uint64_t BufferRecyclerSize,
                                             uint64_t ConnectionRecyclerSize) :
        BackendRecycler(ConnectionRecyclerSize),
        BB(BufferBlockSize, BufferRecyclerSize),
        TCPNoDelay(0),
        TCPNoPush(0) {
}

HTTPConnection *HTTPConnectionBuilder::Get(int SocketFD, SocketAddress *SocketAddress, HTTPServer *Server,
                                           Listening *Listening, SocketEventDomain *EventDomain) {

    HTTPConnection *Connection = BackendRecycler.Get(SocketFD, *SocketAddress);

    if (Connection == nullptr) {
        return nullptr;
    }

    int Open = 1;
    setsockopt(SocketFD, IPPROTO_TCP, TCP_NODELAY, (void *)&Open, sizeof((Open)));

    Connection->ParentServer = Server;
    Connection->ParentListeing = Listening;
    Connection->ParentEventDomain = EventDomain;
    BB.BuildBuffer(Connection->ReadBuffer);
    // configure connection

    return Connection;
}

void HTTPConnectionBuilder::Put(HTTPConnection *C) {
    if (C != nullptr) {
        BackendRecycler.Put(C);
    }
}