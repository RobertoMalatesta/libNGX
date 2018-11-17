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

HTTPListening* HTTPServer::DequeueListening() {

    HTTPListening *L;
    EventError Error{0};

    L = (HTTPListening *)Server::DequeueListening();

    if (L == nullptr) {
        return nullptr;
    }

    Error = EventDomain.DetachSocket(L, SOCK_READ_WRITE_EVENT);

    if (Error.GetCode() != 0) {
        Server::EnqueueListening(L);
        return nullptr;
    }

    L->ParentServer = nullptr;
    L->ParentEventDomain = nullptr;

    return L;
}

EventError HTTPServer::AttachConnection(HTTPConnection *C) {

    EventError Error{0};

    Error = Server::AttachConnection(C);

    if (Error.GetCode() != 0) {
        return Error;
    }

    Error = EventDomain.AttachSocket(C, SOCK_READ_WRITE_EVENT);

    if (Error.GetCode() != 0) {
        Server::DetachConnection(C);
        return Error;
    }

    return {0};
}

EventError HTTPServer::DetachConnection(HTTPConnection *C) {

    EventError Error{0};

    EventDomain.DetachSocket(C, SOCK_READ_WRITE_EVENT);

    if (Error.GetCode() != 0) {
        return Error;
    }

    Error = Server::DetachConnection(C);

    if (Error.GetCode() != 0) {
        Error = EventDomain.AttachSocket(C, SOCK_READ_WRITE_EVENT);
        return Error;
    }

    return {0};

}

RuntimeError HTTPServer::PostProcessFinished() {
    return {0};
}

RuntimeError HTTPServer::PostConnectionEvent(Connection &C, uint32_t EventType) {

    HTTPConnection *HC;

    HC = (HTTPConnection *)(&C);



    return {0, nullptr};
}

RuntimeError HTTPServer::HTTPServerEventProcess() {
    return EventDomain.EventDomainProcess(this);
}


RuntimeError HTTPServer::GetConnection(HTTPConnection *&C, int SocketFD, SocketAddress &Address) {

    C = nullptr;

    if (SocketFD == -1) {
        return {EINVAL, "bad connection fd"};
    }

    if (ConnectionBuilder.Get(C, SocketFD, Address, this, &EventDomain) == -1) {
        return {EINVAL, "can not get connection"};
    }

    return {0};
}

RuntimeError HTTPServer::PutConnection(HTTPConnection *&C) {

    DetachConnection(C);
    C->Reset();
    ConnectionBuilder.Put(C);

    return {0};
}
