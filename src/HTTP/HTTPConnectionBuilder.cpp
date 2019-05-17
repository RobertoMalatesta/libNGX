#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionBuilder::HTTPConnectionBuilder(
        size_t BufferBlockSize,
        uint32_t BufferCollectorSize) :
        BB(BufferBlockSize, BufferCollectorSize), BackendAllocator(),
        NonBlock(true), TCPNoDelay(1), TCPNoPush(0) {
}

int HTTPConnectionBuilder::Get(HTTPConnection *&C, int SocketFD, Address_t &Address, HTTPServer *Server,
                               SocketEventDomain *EventDomain) {

    std::lock_guard<spin_lock> g(Lock);

    if (SocketFD == -1 || Build(C) != 0) {
        return C = nullptr, -1;
    }

    C->SetSocketAddress(SocketFD, Address);

    if (C->SetNonBlock(NonBlock).GetCode() != 0 ||
        C->SetNoDelay(TCPNoDelay == 1).GetCode() != 0 ||
        EventDomain->BindEventThread(*C).GetCode() != 0) {
        Destroy(C);
        return C = nullptr, -1;
    }

    // configure connection
    C->ParentServer = Server;
    C->ParentEventDomain = EventDomain;

    BB.BuildBuffer(C->ReadBuffer);

    return 0;
}

int HTTPConnectionBuilder::Put(HTTPConnection *&C) {
    std::lock_guard<spin_lock> g(Lock);

    C->ParentEventDomain->UnbindEventThread(*C);
    return Destroy(C);
}

void HTTPConnectionBuilder::Reset() {
    std::lock_guard<spin_lock> g(Lock);
    BackendAllocator.reset();
}