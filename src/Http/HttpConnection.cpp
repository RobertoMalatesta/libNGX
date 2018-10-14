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
    OnEventPromise = HttpConnection::OnConnectionEvent;
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
    OnEventPromise = HttpConnection::OnConnectionEvent;
}

void HttpConnection::OnConnectionEvent(void *Arguments, ThreadPool *TPool) {

    EventPromiseArgs *TempArgument;
    EPollEventDomain *EventDomain;
    Socket *TempSocket;
    HttpConnection *TempConnection;
    EventType Type;

    printf("EnterPromise, Arguments: %p\n", Arguments);

    TempArgument = static_cast<EventPromiseArgs *>(Arguments);

    if (TempArgument->UserArguments[3].Ptr == nullptr ||
        TempArgument->UserArguments[4].Ptr == nullptr ||
        TempArgument->UserArguments[6].Ptr == nullptr
        ) {
        return;
    }

    EventDomain = static_cast<EPollEventDomain *>(TempArgument->UserArguments[4].Ptr);
    TempSocket = static_cast<Socket *>(TempArgument->UserArguments[6].Ptr);
    TempConnection = (HttpConnection *)TempSocket;
    Type = static_cast<EventType>(TempArgument->UserArguments[7].UInt);

    printf("Event Type: %x, connection fd: %d\n", Type, TempConnection->GetSocketFD());

    if ((Type & ET_CONNECTED) != 0) {
        EventDomain->AttachSocket(TempConnection, SOCK_READ_WRITE_EVENT);
    }
    if ((Type & ET_READ) != 0) {

        HttpRequestBuffer *Buffer = &TempConnection->RequestBuffer;

        Buffer->WriteDataToBuffer(TempConnection);
        Buffer->ProcessHttpRequest();
        EventDomain->AttachSocket(TempConnection, SOCK_READ_EVENT);
    }
    if ((Type & ET_WRITE) != 0) {
        EventDomain->AttachSocket(TempConnection, SOCK_WRITE_EVENT);
    }

    printf("LeavePromise, Arguments: %p\n", Arguments);
}

void HttpConnection::Reset() {
    RequestBuffer.Reset();
}
