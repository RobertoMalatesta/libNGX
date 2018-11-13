#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection(struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, this),
        TCP4Connection(SocketAddress),
        Request(&MemPool) {
}

HTTPConnection::HTTPConnection(int SocketFd, struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, this),
        TCP4Connection(SocketFd, SocketAddress),
        Request(&MemPool) {
}

void HTTPConnection::OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool) {

    HTTPServer *TargetServer;
    HTTPConnection *TargetConnection;
    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);
    TargetServer = TargetConnection->ParentServer;

    printf("Timer trigger!\n");

    if (TargetConnection->Closed) {
        TargetConnection->Reset();
        TargetServer->PutConnection(TargetConnection);
    } else {
        TargetConnection->Event |= ET_TIMER;
        OnConnectionEvent(PointerToConnection, TPool);
    }
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool) {

    EventType Type;
    HTTPConnection *TargetConnection;
    SocketEventDomain *EventDomain;

    printf("EnterPromise, PointerToConnection: %p\n", PointerToConnection);

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);

    Type = TargetConnection->Event;
    EventDomain = TargetConnection->ParentEventDomain;

    TargetConnection->Event = ET_NONE;

    printf("Event Type: %x, connection fd: %d\n", Type, TargetConnection->GetSocketFD());

    TargetConnection->LastEventTimestamp = GetTimeStamp();

    if ((Type & ET_CONNECTED) != 0) {
        EventDomain->AttachSocket(TargetConnection, SOCK_READ_WRITE_EVENT);
    }
    if ((Type & ET_READ) != 0) {
        Buffer *Buffer = &TargetConnection->ReadBuffer;
        Buffer->WriteConnectionToBuffer(TargetConnection);
    }
    if ((Type & ET_WRITE) != 0) {
        // Mark the socket as writable and write all data to it!
    }
    if ((Type & ET_TIMER) != 0) {
        // Handle timer
    }
    TargetConnection->Lock.Unlock();

    SpinlockGuard LockGuard(&TargetConnection->Lock);
    // Request Process Code
    if ((Type & ET_READ) != 0) {

        HTTPParser::ParseHTTPRequest(TargetConnection->ReadBuffer, TargetConnection->Request);

        const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";

        write(TargetConnection->GetSocketFD(), Content, sizeof(Content) - 1);
        TargetConnection->Close();
    }

    printf("LeavePromise, PointerToConnection: %p\n", PointerToConnection);
}

RuntimeError HTTPConnection::SetSocketAddress(int SocketFD, struct SocketAddress &TargetSocketAddress) {
    this->SocketFd = SocketFD;
    this->SocketAddress = TargetSocketAddress;
    return {0};
}

void HTTPConnection::Reset() {
    ReadBuffer.Reset();
    MemPool.Reset();
}

SocketError HTTPConnection::Close() {

    uint64_t RecycleTime;
    ParentEventDomain->DetachSocket(this, SOCK_READ_WRITE_EVENT);

    if (SocketFd != -1) {
        close(SocketFd);
        SocketFd = -1;
    }

    Closed = true;

    RecycleTime = GetTimeStamp() + 5;

    TimerNode.SetExpireTime(RecycleTime);
    ParentEventDomain->AttachTimer(TimerNode);
    return {0};
}

RuntimeError HTTPConnection::RefreshTimer() {
    return {0};
}
