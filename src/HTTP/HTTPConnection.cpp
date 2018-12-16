#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnection::HTTPConnection() :
        TCP4Connection(Address),
        Request(nullptr),
        EventJob(HTTPConnection::OnConnectionEvent, this) {
    TimerNode = {0, HTTPConnection::OnTimerEvent, this};
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

    return PostJob(EventJob);
}

void HTTPConnection::OnTimerEvent(void *PointerToConnection) {

    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->Event |= ET_TIMER;

    LOG(INFO) << "in timer func, fd: "<< C->SocketFD;

    HTTPConnection::OnConnectionEvent(PointerToConnection);
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection) {

    EventType Type;

    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);

    LOG(INFO) << "in connection event, fd: "<< C->SocketFD;

    Type = C->Event;

    if (C->Open == 1) {

        // Sequence
        //
        // 1.   Read Buffer
        // 2.   Process Request
        // 3.   Handle Write Buffer
        // 4.   Handle Timer

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

        if (Type & ET_TIMER) {
            LOG(INFO) << "recycle connection, fd: "<< C->SocketFD;
            C-> ParentServer->PutConnection(C);
        }
    }
}

void HTTPConnection::Reset() {

    // Clear all attached events
    AttachedEvent = Event = 0;

    // Reset read buffer
    ReadBuffer.Reset();

    // Reset memory pool
    MemPool.Reset();

    // Reset timer node
    TimerNode.Reset();
}

SocketError HTTPConnection::Close() {

    HTTPConnection *C = this;

    // Detach read write event
    ParentEventDomain->DetachSocket(*this, ET_READ | ET_WRITE);

    // Close TCP Connection
    TCP4Connection::Close();

    // Set a timer to put Connection to recycler
    ParentEventDomain->SetTimer(*C, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);

    return {0};
}
