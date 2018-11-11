#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection(struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnConnectionEvent, nullptr),
        TCP4Connection(SocketAddress),
        Request(&MemPool) {
}

HTTPConnection::HTTPConnection(int SocketFd, struct SocketAddress &SocketAddress) :
        TimerNode(0, HTTPConnection::OnConnectionEvent, nullptr),
        TCP4Connection(SocketFd, SocketAddress),
        Request(&MemPool) {
}

void HTTPConnection::OnCloseConnection(void *PointerToConnection, ThreadPool *TPool){

    HTTPConnection *TargetConnection;
    HTTPServer *Server;

    printf("EnterPromise OnCloseConnection, PointerToConnection: %p\n", PointerToConnection);

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);
    Server = TargetConnection->ParentServer;

    Server->CloseConnection(TargetConnection);

    printf("LeavePromise OnClose Connection, PointerToConnection: %p\n", PointerToConnection);
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool) {

    EventType Type;
    HTTPConnection *TargetConnection;
    SocketEventDomain *EventDomain;

    printf("EnterPromise, PointerToConnection: %p\n", PointerToConnection);

    TargetConnection = static_cast<HTTPConnection *>(PointerToConnection);
    EventDomain = TargetConnection->ParentEventDomain;
    Type = TargetConnection->Event;

    printf("Event Type: %x, connection fd: %d\n", Type, TargetConnection->GetSocketFD());

    TargetConnection->Lock.Unlock();

    if ((Type & ET_CONNECTED) != 0) {
        EventDomain->AttachSocket(TargetConnection, SOCK_READ_WRITE_EVENT);
    }
    if ((Type & ET_READ) != 0) {
        Buffer *Buffer = &TargetConnection->ReadBuffer;
        Buffer->WriteConnectionToBuffer(TargetConnection);
        HTTPParser::ParseHTTPRequest(TargetConnection->ReadBuffer, TargetConnection->Request);

        const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";


        EventDomain->DetachSocket(TargetConnection, SOCK_READ_WRITE_EVENT);
        write(TargetConnection->GetSocketFD(), Content, sizeof(Content) - 1);
        EventDomain->PostTimerEvent(1, HTTPConnection::OnCloseConnection, TargetConnection);
    }
    if ((Type & ET_WRITE) != 0) {
        // Mark the socket as writable and write all data to it!
    }



    printf("LeavePromise, PointerToConnection: %p\n", PointerToConnection);
}

RuntimeError HTTPConnection::SetSocketAddress(int SocketFD, struct SocketAddress &TargetSocketAddress) {
    this->SocketFd = SocketFD;
    this->SocketAddress = TargetSocketAddress;
    return {0};
}

void HTTPConnection::Reset() {

    if (SocketFd != -1) {
        close(SocketFd);
        SocketFd = -1;
    }

    ReadBuffer.Reset();
    MemPool.Reset();
}
