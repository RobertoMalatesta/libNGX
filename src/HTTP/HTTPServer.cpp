#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPServer::HTTPServer(
        size_t PoolSize,
        int ThreadCount,
        int EPollSize,
        size_t BufferBlockSize,
        uint64_t ConnectionRecycleSize,
        uint64_t BufferRecycleSize) :
        Server(),
        ConnectionBuilder(BufferBlockSize, BufferRecycleSize, ConnectionRecycleSize),
        EventDomain(PoolSize, ThreadCount, EPollSize) {

}

EventError HTTPServer::EnqueueListening(HTTPListening *L) {

    EventError Error{0};

    Error = EventDomain.AttachSocket(L, SOCK_READ_WRITE_EVENT);

    if (Error.GetCode() != 0) {
        return Error;
    }

    Error = Server::EnqueueListening(L);

    if (Error.GetCode() != 0) {
        EventDomain.DetachSocket(L, SOCK_READ_WRITE_EVENT);
        return Error;
    }

    L->ParentServer = this;
    L->ParentEventDomain = &EventDomain;

    return {0};
}

EventError HTTPServer::DequeueListening(HTTPListening *&L) {

    EventError Error{0};

    L = (HTTPListening *)Server::DequeueListening();

    if (L == nullptr) {
        return {ENOENT, "no listening in queue"};
    }

    Error = EventDomain.DetachSocket(L, SOCK_READ_WRITE_EVENT);

    if (Error.GetCode() != 0) {
        Server::EnqueueListening(L);
        return Error;
    }

    L->ParentServer = nullptr;
    L->ParentEventDomain = nullptr;

    return {0};
}

RuntimeError HTTPServer::PostProcessFinished() {
    return {0};
}

RuntimeError HTTPServer::HTTPServerEventProcess() {
    return EventDomain.EventDomainProcess(this);
}


RuntimeError HTTPServer::GetConnection(HTTPConnection *&C, int SocketFD, SocketAddress &Address) {

    C = nullptr;

    if (SocketFD == -1) {
        return {EINVAL, "bad connection fd"};
    }

    if (ConnectionBuilder.Get(C, SocketFD, Address, this, &EventDomain) != 0) {
        return {EINVAL, "can not get connection"};
    }

    return {0};
}

RuntimeError HTTPServer::PutConnection(HTTPConnection *&C) {
    ConnectionBuilder.Put(C);
    return {0};
}
