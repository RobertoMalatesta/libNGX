#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(u_char Lg2BufferBlockSize, uint64_t BufferRecycleBinSize,
                                             uint64_t ConnectionRecycleBinSize) :
        BackendRecycleBin(ConnectionRecycleBinSize),
        BB(Lg2BufferBlockSize, BufferRecycleBinSize),
        TCPNoDelay(1), TCPNoPush(0) {

}

int HTTPConnectionBuilder::Get(HTTPConnection *&C, int SocketFD, SocketAddress &SocketAddress, HTTPServer *Server,
                               SocketEventDomain *EventDomain) {

    LockGuard LockGuard(&Lock);

    if (SocketFD == -1 || BackendRecycleBin.Get(C, SocketFD, SocketAddress) != 0) {
        return C = nullptr, -1;
    }

    // configure connection
    C->Open = 1;
    C->SetNonBlock(true);
    C->SetNoDelay(TCPNoDelay == 1);
    C->ParentServer = Server;
    C->ParentEventDomain = EventDomain;

    BB.BuildBuffer(C->ReadBuffer);

    return 0;
}

int HTTPConnectionBuilder::Put(HTTPConnection *&C) {
    LockGuard LockGuard(&Lock);
    return BackendRecycleBin.Put(C);
}