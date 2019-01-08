#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(size_t BufferBlockSize, uint64_t BufferRecycleBinSize,
                                             uint64_t ConnectionRecycleBinSize) :
        BackendRecycleBin(ConnectionRecycleBinSize),
        BB(BufferBlockSize, BufferRecycleBinSize),
        TCPNoDelay(1), TCPNoPush(0) {

}

int HTTPConnectionBuilder::Get(HTTPConnection *&C, int SocketFD, SocketAddress &Address, HTTPServer *Server,
                               SocketEventDomain *EventDomain) {

    LockGuard LockGuard(&Lock);

    if (SocketFD == -1 || BackendRecycleBin.Get(C, SocketFD, Address) != 0) {
        return C = nullptr, -1;
    }

    if (C->SetNonBlock(true).GetCode() != 0 ||
        C->SetNoDelay(TCPNoDelay == 1).GetCode() != 0 ||
        EventDomain->BindEventThread(*C).GetCode() != 0) {
        BackendRecycleBin.Put(C);
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
    return BackendRecycleBin.Put(C);
}