#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection() :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, this),
        TCP4Connection(Address),
        Request(&MemPool) {
}

HTTPConnection::HTTPConnection(struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, this),
        TCP4Connection(SocketAddress),
        Request(&MemPool) {
}

HTTPConnection::HTTPConnection(int SocketFD, struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, this),
        TCP4Connection(SocketFD, SocketAddress),
        Request(&MemPool) {
}

RuntimeError HTTPConnection::SetSocketAddress(int SocketFD, struct SocketAddress &Address) {

    SpinlockGuard LockGuard(&Lock);

    this->SocketFD = SocketFD;
    this->Address = Address;

    return {0};
}

RuntimeError HTTPConnection::HandleEventDomain(uint32_t EventType) {

    Lock.Lock();
    Event = EventType;

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "connection not attached"};
    } else if (Open == 1) {
        return ParentEventDomain->PostPromise(HTTPConnection::OnConnectionEvent, static_cast<void *>(this));
    }

    return {EFAULT, "connection not open"};

}

void HTTPConnection::OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool) {

    HTTPConnection *TargetConnection;

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);

    if (TargetConnection->Open == 1) {
        TargetConnection->Lock.Lock();
        OnConnectionEvent(PointerToConnection, TPool);
    } else {
        TargetConnection->Lock.Unlock();

        printf("Recycle connection: %p\n", TargetConnection);

        TargetConnection->ParentServer->PutConnection(TargetConnection);
        TargetConnection->ParentEventDomain->ResetTimer(TargetConnection->TimerNode);
    }
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *) {

    EventType Type;
    HTTPConnection *C;

    printf("EnterPromise, PointerToConnection: %p\n", PointerToConnection);

    C = static_cast<HTTPConnection *>(PointerToConnection);

    Type = C->Event;

    printf("Event Type: %x, connection fd: %d\n", Type, C->GetSocketFD());


    if ((Type & ET_READ) != 0) {
        Buffer *Buffer = &C->ReadBuffer;
        Buffer->WriteConnectionToBuffer(C);
    }

    if ((Type & ET_WRITE) != 0) {
        // Mark the socket as writable and write all data to it!
    }

    if ((Type & ET_TIMER) != 0) {
        printf("In timer event process: %p\n", C);
        // Handle timer
    }

    C->Lock.Unlock();

    // Handle Request here

    if ((Type & ET_READ) != 0) {
        {
            SpinlockGuard LockGuard(&C->Lock);

            HTTPParser::ParseHTTPRequest(C->ReadBuffer, C->Request);

            const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";

            write(C->GetSocketFD(), Content, sizeof(Content) - 1);
        }
        C->Close();
    }

    printf("LeavePromise, PointerToConnection: %p\n", PointerToConnection);
}

void HTTPConnection::Reset() {

    SpinlockGuard LockGuard(&Lock);

    ReadBuffer.Reset();
    MemPool.Reset();
}

SocketError HTTPConnection::Close() {

    SpinlockGuard LockGuard(&Lock);

    ParentServer->DetachConnection(*this);

    if (SocketFD != -1 || Open == 1) {
        close(SocketFD);
        SocketFD = -1, Open = 0;
    }

    ParentEventDomain->SetTimer(TimerNode, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);
    return {0};
}
