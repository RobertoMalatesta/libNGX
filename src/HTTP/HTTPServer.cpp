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

EventError HTTPServer::AttachListening(HTTPListening &L) {

    EventError Error{0};
    L.Lock();
    Error = EventDomain.AttachSocket(L, ET_READ | ET_WRITE | ET_ACCEPT);
    L.Unlock();

    if (Error.GetCode() != 0) {
        return Error;
    }

    L.ParentServer = this;
    L.ParentEventDomain = &EventDomain;

    return {0};
}

EventError HTTPServer::DetachListening(HTTPListening &L) {

    EventError Error{0};

    L.Lock();
    Error = EventDomain.DetachSocket(L, ET_READ | ET_WRITE | ET_ACCEPT);
    L.Unlock();

    if (Error.GetCode() != 0) {
        return Error;
    }

    L.ParentServer = nullptr;
    L.ParentEventDomain = nullptr;

    return {0};
}

RuntimeError HTTPServer::PostProcessFinished() {
    return {0};
}

RuntimeError HTTPServer::ServerEventProcess() {
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
