#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpConnection::HttpConnection(
        struct sockaddr *SocketAddress,
                socklen_t SocketLength,
                BufferMemoryBlockRecycler *Recycler,
                size_t BufferBlockSize
                        ):
        TCP4Connection(SocketAddress, SocketLength),
        RequestBuffer(Recycler, BufferBlockSize){
    Lock.Unlock();
    OnEvent = & HttpConnection::OnEventFunc;
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
    OnEvent = & HttpConnection::OnEventFunc;
}

char buffer[BUFSIZ];

void HttpConnection::OnEventFunc(void *Argument, ThreadPool *TPool) {

    HttpServer *Server;
    EPollEventDomain *EventDomain;
    EPollEventDomainArgument *EventDomainArguments;
    HttpConnection *Connection;

    if (Argument == nullptr) {
        return;
    }

    EventDomainArguments = static_cast<EPollEventDomainArgument *>(Argument);
    EventDomain = EventDomainArguments->EventDomain;
    Connection = static_cast<HttpConnection *>(EventDomainArguments->UserArguments[1].Ptr);
    Server = static_cast<HttpServer *>(EventDomainArguments->UserArguments[0].Ptr);

    if (EventDomain == nullptr || Connection == nullptr) {
        return;
    }

    printf("Start Processing Event( FD: %d, EV: 0x%08X )\n", Connection->GetSocketFD(), EventDomainArguments->UserArguments[2].UInt);

    if (EventDomainArguments->UserArguments[2].UInt & ET_CONNECTED) {
        EventDomain->AttachSocket(Connection, SOCK_READ_WRITE_EVENT);
        Server->AttachConnection(Connection);
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_READ) {
        EventDomain->DetachSocket(Connection, SOCK_READ_EVENT);
        printf("%s:%s\n", __FUNCTION__, Connection->RequestBuffer.WriteDataToBuffer(Connection).GetErrorString());
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_WRITE) {
        EventDomain->DetachSocket(Connection, SOCK_WRITE_EVENT);
    }
    printf("End Processing Event( FD: %d, EV: 0x%08X )\n", Connection->GetSocketFD(), EventDomainArguments->UserArguments[2].UInt);
}

void HttpConnection::Reset() {



}