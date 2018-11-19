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

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "connection not attached"};
    }

    Lock.Lock();
    Event = EventType;

    if (Open == 0) {
        SocketFD = -1;
        Lock.Unlock();
        return {EFAULT, "connection not open"};
    };

    return ParentEventDomain->PostPromise(HTTPConnection::OnConnectionEvent, static_cast<void *>(this));
}

void HTTPConnection::OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool) {

    HTTPConnection *TargetConnection;

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);
    TargetConnection->Lock.Lock();

    TargetConnection->Event |= ET_TIMER;

    if (TargetConnection->Open == 1) {
        OnConnectionEvent(PointerToConnection, TPool);
    } else {

        printf("recycle connection: %p\n", TargetConnection);

        TargetConnection->Lock.Unlock();
        TargetConnection->ParentServer->PutConnection(TargetConnection);
    }
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *) {

    EventType Type;
    HTTPConnection *C;

    printf("Enter Promise, PointerToConnection: %p\n", PointerToConnection);

    C = static_cast<HTTPConnection *>(PointerToConnection);

    Type = C->Event;

    printf("Event Type: %x, connection fd: %d\n", Type, C->GetSocketFD());


    if ((Type & ET_READ) != 0) {
        C->ReadBuffer.ReadFromConnection(C);
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

            ssize_t size = write(C->GetSocketFD(), Content, sizeof(Content) - 1);

            if ( size <= 0 ) {
                printf("bad write!\n");
            }
        }
        C->Close();
    }

    printf("Leave Promise, PointerToConnection: %p\n", PointerToConnection);
}

void HTTPConnection::Reset() {

    SpinlockGuard LockGuard(&Lock);

    ParentEventDomain->ResetTimer(TimerNode);
    ReadBuffer.Reset();
    MemPool.Reset();
}

SocketError HTTPConnection::Close() {

    SpinlockGuard LockGuard(&Lock);

    ParentEventDomain->DetachSocket(*this, ET_READ | ET_WRITE);

    if (SocketFD != -1 || Open == 1) {

        if (close(SocketFD) == -1) {
            printf("failed to close socket!\n");
        }
        SocketFD = -1, Open = 0;
    }

    ParentEventDomain->SetTimer(TimerNode, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);
    return {0};
}
