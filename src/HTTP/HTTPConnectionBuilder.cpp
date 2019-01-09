#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(
        size_t BufferBlockSize,
        uint32_t BufferCollectorSize,
        uint32_t ConnectionCollectorSize) :
        BB(BufferBlockSize, BufferCollectorSize), TCPNoDelay(1), TCPNoPush(0),
        BackendCollector(ConnectionCollectorSize) {
}

int HTTPConnectionBuilder::Get(HTTPConnection *&C, int SocketFD, SocketAddress &Address, HTTPServer *Server,
                               SocketEventDomain *EventDomain) {

    LockGuard LockGuard(&Lock);

    if (SocketFD == -1 || BackendCollector.Get(C, SocketFD, Address) != 0) {
        return C = nullptr, -1;
    }

    if (C->SetNonBlock(true).GetCode() != 0 ||
        C->SetNoDelay(TCPNoDelay == 1).GetCode() != 0 ||
        EventDomain->BindEventThread(*C).GetCode() != 0) {
        BackendCollector.Put(C);
        return C = nullptr, -1;
    }

    // configure connection
    C->ParentServer = Server;
    C->ParentEventDomain = EventDomain;

    BB.BuildBuffer(C->ReadBuffer);

    return 0;
}

int HTTPConnectionBuilder::Put(HTTPConnection *&C) {
    LockGuard LockGuard(&Lock);
    C->ParentEventDomain->UnbindEventThread(*C);
    return BackendCollector.Put(C);
}