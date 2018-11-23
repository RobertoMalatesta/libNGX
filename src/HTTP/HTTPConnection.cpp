#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection() :
        TCP4Connection(Address),
        Request(nullptr) {
    TimerNode.Callback = HTTPConnection::OnTimerEvent;
    TimerNode.Argument = this;

    Type = SOCK_TYPE_STREAM, Version = 4, IsListen = 1;
}

RuntimeError HTTPConnection::SetSocketAddress(int SocketFD, struct SocketAddress &Address) {

    LockGuard Guard(&SocketLock);
    this->SocketFD = SocketFD;
    this->Address = Address;

    return {0};
}

RuntimeError HTTPConnection::HandleEventDomain(uint32_t EventType) {

    LockGuard Guard(&SocketLock);

    LOG(INFO) << "handle HTTPConnection event, fd: "<< SocketFD << ", type: " << EventType;

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        LOG(WARNING) << "connection not attached, fd: "<< SocketFD;
        return {EINVAL, "connection not attached"};
    }

    Event = EventType;

    if (Open == 0) {
        SocketFD = -1;
        LOG(WARNING) << "connection not open, fd: "<< SocketFD;
        return {EFAULT, "connection not open"};
    };

    return ParentEventDomain->PostPromise(HTTPConnection::OnConnectionEvent, static_cast<void *>(this));
}

void HTTPConnection::OnTimerEvent(void *PointerToConnection, ThreadPool *TPool) {

    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->Lock();
    C->Event |= ET_TIMER;

    LOG(INFO) << "in timer func, fd: "<< C->SocketFD;

    if (C->Open == 0) {
        LOG(INFO) << "recycle connection, fd: "<< C->SocketFD;
        C-> ParentServer->PutConnection(C);
    }

    if (C != nullptr) {
        C->Unlock();
    }
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *) {

    EventType Type;
    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->SocketLock.Lock();

    LOG(INFO) << "in connection event, fd: "<< C->SocketFD;

    if (C->Open == 1) {
        Type = C->Event;
        LOG(INFO) << "handle connection event, fd: "<< C->SocketFD << ", event type: " << Type;

        if ((Type & ET_READ) != 0) {
            C->ReadBuffer.ReadFromConnection(C);
        }

        if ((Type & ET_WRITE) != 0) {
            // Mark the socket as writable and write all data to it!
        }

        // Handle Request here

        LOG(INFO) << "writing data to connection, fd: " << C->SocketFD;
//            HTTPParser::ParseHTTPRequest(C->ReadBuffer, C->Request);
        const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";

        ssize_t size = write(C->GetSocketFD(), Content, sizeof(Content) - 1);

        if (size < 0) {
            LOG(INFO) << "bad write size < 0, fd: " << C->SocketFD;
        }
        LOG(INFO) << "will close socket: " << C->SocketFD;
        C->Close();
    } else {
        LOG(INFO) << "connection already closed, skip it`s event" << C->SocketFD;
    }

    C->SocketLock.Unlock();
}

void HTTPConnection::Reset() {
    AttachedEvent = Event = 0;
    ReadBuffer.Reset();
    MemPool.Reset();
    TimerNode.Reset();
}

SocketError HTTPConnection::Close() {

    HTTPConnection *C = this;

    ParentEventDomain->DetachSocket(*this, ET_READ | ET_WRITE);
    TCP4Connection::Close();
    ParentEventDomain->SetTimer(*C, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);

    return {0};
}
