#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection(struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, nullptr),
        TCP4Connection(SocketAddress),
        Request(&MemPool) {
}

HTTPConnection::HTTPConnection(int SocketFd, struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnTimerEventWarp, nullptr),
        TCP4Connection(SocketFd, SocketAddress),
        Request(&MemPool) {
}

void HTTPConnection::OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool) {
    HTTPConnection *TargetConnection;

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);

    TargetConnection->Lock.Lock();
    TargetConnection->Event = ET_TIMER;
    OnConnectionEvent(PointerToConnection, TPool);
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool) {

    EventType Type;
    HTTPConnection *TargetConnection;
    SocketEventDomain *EventDomain;
    HTTPServer *TargetServer;

    printf("EnterPromise, PointerToConnection: %p\n", PointerToConnection);

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);

    Type = TargetConnection->Event;
    EventDomain = TargetConnection->ParentEventDomain;
    TargetServer = TargetConnection->ParentServer;

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
        if (TargetConnection->Closed) {
            TargetServer->PutConnection(TargetConnection);
        }
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

    ParentEventDomain->DetachTimer(TimerNode);
    TimerNode.SetExpireTime(RecycleTime);
    ParentEventDomain->AttachTimer(TimerNode);

    return {0};
}

RuntimeError HTTPConnection::RefreshTimer() {
    return {0};
}
