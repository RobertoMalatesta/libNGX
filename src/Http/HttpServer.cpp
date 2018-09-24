#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpConnection::HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength):
        TCP4Connection(SocketAddress, SocketLength){
    Lock.Unlock();
    OnEvent = & HttpConnection::OnEventFunc;
}

HttpConnection::HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength) :
        TCP4Connection(SocketFd, SocketAddress, SocketLength){
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
    printf("event start: %p, %p, %x \n", EventDomain, Connection, EventDomainArguments->UserArguments[2].UInt);

    if (EventDomain == nullptr || Connection == nullptr) {
        return;
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_CONNECTED) {
        cout<< "Connected: "<< Connection->GetSocketFD() << endl;
        EventDomain->AttachSocket(Connection, SOCK_READ_WRITE_EVENT);
        Server->AttachConnection(Connection);
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_READ) {
        EventDomain->DetachSocket(Connection, SOCK_READ_EVENT);
        cout<< "Read: "<< Connection->GetSocketFD() << endl;
    }

    if (EventDomainArguments->UserArguments[2].UInt & ET_WRITE) {
        EventDomain->DetachSocket(Connection, SOCK_WRITE_EVENT);
        cout<< "Write: "<< Connection->GetSocketFD() << endl;
    }
    printf("event end: %p, %p\n", EventDomain, Connection);
}

HttpServer::HttpServer(size_t PoolSize, int ThreadCount, int EPollSize,
        int ConnectionRecycleSize, int BufferRecycleSize) : Server(),
        EventDomain(PoolSize, ThreadCount, EPollSize, HttpServer::HttpEventProcessPromise) {}

void HttpServer::HttpEventProcessPromise(void *Args, ThreadPool *TPool) {

    Socket *S;
    HttpConnection *C;
    HttpServer *Server;
    epoll_event *Events;
    Listening *Listening;
    socklen_t SocketLength;
    EPollEventDomain *EventDomain;
    struct sockaddr_in SocketAddress;
    int EventCount, ConnectionFd;
    EPollEventDomainArgument *EventArgument, *TempArgument;

    if (nullptr == Args) {
        return;
    }

    EventArgument = static_cast<EPollEventDomainArgument *>(Args);

    Events = EventArgument->Events;
    Listening = EventArgument->Listening;
    EventCount = EventArgument->EventCount;
    EventDomain = EventArgument->EventDomain;
    Server = static_cast<HttpServer *>(EventArgument->UserArguments[0].Ptr);

    if (EventDomain == nullptr || Events == nullptr) {
        return;
    }

    for (int i = 0; i<EventCount; i++) {
        S = (Socket *) Events[i].data.ptr;

        if (S == nullptr) {
            continue;
        }

        TempArgument = static_cast<EPollEventDomainArgument *>(EventDomain->Allocate(sizeof(EPollEventDomainArgument)));

        if (TempArgument == nullptr) {
            continue;
        }

        memcpy(TempArgument, EventArgument, sizeof(EPollEventDomainArgument));

        TempArgument->UserArguments[0].Ptr = Server;
        TempArgument->UserArguments[2].UInt = 0;
        TempArgument->EventDomain = EventDomain;

        if (S->GetSocketFD() == Listening->GetSocketFD()) {

            ConnectionFd = accept4(Listening->GetSocketFD(), (struct sockaddr *)&SocketAddress, &SocketLength, SOCK_NONBLOCK);

            if ( -1 == ConnectionFd) {
                printf("accept4() failed in HttpEventProcessPromise!\n");
            }

            C = new HttpConnection(ConnectionFd, (struct sockaddr *)&SocketAddress, SocketLength);
            TempArgument->UserArguments[1].Ptr = C;
            TempArgument->UserArguments[2].UInt |= ET_CONNECTED;
        }
        else {
            C= (HttpConnection *)S;
            TempArgument->UserArguments[1].Ptr = C;
            if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
                TempArgument->UserArguments[2].UInt |= ET_READ;
            }
            if (Events[i].events & (EPOLLOUT)){
                TempArgument->UserArguments[2].UInt |= ET_WRITE;
            }
        }
        TPool->PostPromise(*C->OnEvent, (void *)TempArgument);
    }

    Server->EnqueueListening(Listening);
    EventDomain->Free((void **)&Events);
}

RuntimeError HttpServer::HttpServerEventProcess() {

    Listening *Listen;

    EPollEventDomainArgument *Argument;

    Argument = (EPollEventDomainArgument *)EventDomain.Allocate(sizeof(EPollEventDomainArgument));
    memset(Argument, 0, sizeof(EPollEventDomainArgument));

    Listen = DequeueListening();

    if (Listen == nullptr) {
        Lock.Unlock();
        return RuntimeError(ENOENT, "Can not get A Listening from HttpServer!");
    }

    Lock.Lock();
    Argument->UserArguments[0].Ptr = this;
    Argument->Listening = Listen;
    Argument->EventDomain = &EventDomain;
    Lock.Unlock();

    RuntimeError Error = EventDomain.EventDomainProcess((void *)Argument);

    if (Error.GetErrorCode() == 0) {
        EnqueueListening(Listen);
    }

    return Error;
}