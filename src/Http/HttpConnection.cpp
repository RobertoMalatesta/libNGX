#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpConnection::HttpConnection(
        struct sockaddr *SocketAddress,
                socklen_t SocketLength,
                BufferMemoryBlockRecycler *Recycler,
                size_t BufferBlockSize):

        TCP4Connection(SocketAddress, SocketLength),
        RequestBuffer(Recycler, BufferBlockSize){

    Lock.Unlock();
    OnConnected = HttpConnection::OnConnectedEvent;
    OnRead = HttpConnection::OnReadEvent;
    OnWrite = HttpConnection::OnWriteEvent;
    OnClosed = HttpConnection::OnClosedEvent;
}

HttpConnection::HttpConnection(
        int SocketFd,
        struct sockaddr *SocketAddress,
                socklen_t SocketLength,
        BufferMemoryBlockRecycler *Recycler,
        size_t BufferBlockSize
        ) :
        TCP4Connection(SocketFd, SocketAddress, SocketLength),
        RequestBuffer(Recycler, BufferBlockSize){
    Lock.Unlock();

    OnConnected = HttpConnection::OnConnectedEvent;
    OnRead = HttpConnection::OnReadEvent;
    OnWrite = HttpConnection::OnWriteEvent;
    OnClosed = HttpConnection::OnClosedEvent;
}

void HttpConnection::OnConnectedEvent(void *Arguments, ThreadPool *TPool) {

    void *TempPointer;
    HttpServer *Server;
    EventPromiseArgs *TempArgument;
    EPollEventDomain *EventDomain;
    HttpConnection *Connection;

    printf("EnterPromise: Connected, Arguments: %p\n", Arguments);

    TempArgument = static_cast<EventPromiseArgs *>(Arguments);

    if (TempArgument->UserArguments[3].Ptr == nullptr ||
        TempArgument->UserArguments[4].Ptr == nullptr ||
        TempArgument->UserArguments[6].Ptr == nullptr
        ) {
        return;
    }

    Server = static_cast<HttpServer *>(TempArgument->UserArguments[3].Ptr);
    EventDomain = static_cast<EPollEventDomain *>(TempArgument->UserArguments[4].Ptr);
    Connection = static_cast<HttpConnection *>(TempArgument->UserArguments[6].Ptr);

    EventDomain->AttachSocket(Connection, SOCK_READ_WRITE_EVENT);

    printf("LeavePromise: Connected, Arguments: %p\n", Arguments);
}

void HttpConnection::OnReadEvent(void *Arguments, ThreadPool *TPool) {

    void *TempPointer;
    HttpServer *Server;
    EventPromiseArgs *TempArgument;
    EPollEventDomain *EventDomain;
    HttpConnection *Connection;

    printf("EnterPromise: Read, Arguments: %p\n", Arguments);

    TempArgument = static_cast<EventPromiseArgs *>(Arguments);

    if (TempArgument->UserArguments[3].Ptr == nullptr ||
        TempArgument->UserArguments[4].Ptr == nullptr ||
        TempArgument->UserArguments[6].Ptr == nullptr
        ) {
        return;
    }

    Server = static_cast<HttpServer *>(TempArgument->UserArguments[3].Ptr);
    EventDomain = static_cast<EPollEventDomain *>(TempArgument->UserArguments[4].Ptr);
    Connection = static_cast<HttpConnection *>(TempArgument->UserArguments[6].Ptr);

    Connection->ReadConnection();
    EventDomain->AttachSocket(Connection, SOCK_READ_EVENT);

    printf("LeavePromise: Read, Arguments: %p\n", Arguments);
}

void HttpConnection::OnWriteEvent(void *Arguments, ThreadPool *TPool) {

    void *TempPointer;
    HttpServer *Server;
    EventPromiseArgs *TempArgument;
    EPollEventDomain *EventDomain;
    HttpConnection *Connection;

    printf("EnterPromise: Write, Arguments: %p\n", Arguments);

    TempArgument = static_cast<EventPromiseArgs *>(Arguments);

    if (TempArgument->UserArguments[3].Ptr == nullptr ||
        TempArgument->UserArguments[4].Ptr == nullptr ||
        TempArgument->UserArguments[6].Ptr == nullptr
        ) {
        return;
    }
    Server = static_cast<HttpServer *>(TempArgument->UserArguments[3].Ptr);
    EventDomain = static_cast<EPollEventDomain *>(TempArgument->UserArguments[4].Ptr);
    Connection = static_cast<HttpConnection *>(TempArgument->UserArguments[6].Ptr);

    EventDomain->DetachSocket(Connection, SOCK_WRITE_EVENT);

    printf("LeavePromise: Write, Arguments: %p\n", Arguments);
}

void HttpConnection::OnClosedEvent(void *Arguments, ThreadPool *TPool) {

    void *TempPointer;
    HttpServer *Server;
    EventPromiseArgs *TempArgument;
    EPollEventDomain *EventDomain;
    HttpConnection *Connection;

    printf("EnterPromise: Closed, Arguments: %p\n", Arguments);

    TempArgument = static_cast<EventPromiseArgs *>(Arguments);

    if (TempArgument->UserArguments[3].Ptr == nullptr ||
        TempArgument->UserArguments[4].Ptr == nullptr ||
        TempArgument->UserArguments[6].Ptr == nullptr
        ) {
        return;
    }

    Server = static_cast<HttpServer *>(TempArgument->UserArguments[3].Ptr);
    EventDomain = static_cast<EPollEventDomain *>(TempArgument->UserArguments[4].Ptr);
    Connection = static_cast<HttpConnection *>(TempArgument->UserArguments[6].Ptr);

    printf("LeavePromise: Closed, Arguments: %p\n", Arguments);
}

void HttpConnection::Reset() {
    RequestBuffer.Reset();
}
