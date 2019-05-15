#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPServer::HTTPServer(size_t BufferBlockSize,
                       uint32_t ConnectionCollectorSize,
                       uint32_t BufferCollectorSize,
                       SocketEventDomain *EventDomain) :
        Server(EventDomain),
        ConnectionBuilder(BufferBlockSize, BufferCollectorSize) {
}

EventError HTTPServer::AttachListening(HTTPListening &L) {

    EventError Error{0};

    if (EventDomain == nullptr) {
        return {EINVAL, "backend SocketEventDomain not available"};
    }

    L.Lock();
    Error = EventDomain->AttachSocket(L, ET_READ | ET_WRITE | ET_ACCEPT);
    L.Unlock();

    if (Error.GetCode() != 0) {
        return Error;
    }

    L.ParentServer = this;

    return {0};
}

EventError HTTPServer::DetachListening(HTTPListening &L) {

    EventError Error{0};

    if (EventDomain == nullptr) {
        return {EINVAL, "backend SocketEventDomain not available"};
    }

    L.Lock();
    Error = EventDomain->DetachSocket(L, ET_READ | ET_WRITE | ET_ACCEPT);
    L.Unlock();

    if (Error.GetCode() != 0) {
        return Error;
    }

    L.ParentServer = nullptr;

    return {0};
}

RuntimeError HTTPServer::OnLoopFinished() {
    return {0};
}

RuntimeError HTTPServer::ServerEventLoop() {

    RuntimeError Error(0);

    if (EventDomain == nullptr) {
        return {EINVAL, "backend SocketEventDomain not available"};
    }

    Error = EventDomain->EventLoop();

    if (Error.GetCode() != 0) {
        return Error;
    }

    return OnLoopFinished();
}


RuntimeError HTTPServer::GetConnection(HTTPConnection *&C, int SocketFD, Address_t &Address) {

    C = nullptr;

    if (SocketFD == -1) {
        return {EINVAL, "bad connection fd"};
    }

    if (ConnectionBuilder.Get(C, SocketFD, Address, this, EventDomain) != 0) {
        return {EINVAL, "can not get connection"};
    }

    return {0};
}

RuntimeError HTTPServer::PutConnection(HTTPConnection *&C) {
    ConnectionBuilder.Put(C);
    return {0};
}
