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

void HttpConnection::OnEventFunc(void *Argument, ThreadPool *TPool) {

    HttpEventArgument *HttpArguments;
    EPollEventDomain *EventDomain;
    HttpConnection *Connection;

    cout<<"event"<<endl;

    if (Argument == nullptr) {
        return;
    }

    HttpArguments = static_cast<HttpEventArgument *>(Argument);

    if (HttpArguments->EventDomain == nullptr || HttpArguments->Connection == nullptr) {
        return;
    }

    EventDomain = (EPollEventDomain *)HttpArguments->EventDomain;
    Connection = HttpArguments->Connection;

    if (HttpArguments->Connected) {
        cout<< "Connected: "<< Connection->GetSocketFD() << endl;
    }

    if (HttpArguments->Read) {
        cout<< "Read: "<< Connection->GetSocketFD() << endl;
    }

    if (HttpArguments->Write) {
        cout<< "Write: "<< Connection->GetSocketFD() << endl;
    }
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
    EPollEventDomain *Domain;
    struct sockaddr_in SocketAddress;
    int EventCount, ConnectionFd;
    HttpEventArgument *HttpEventArgument, *TempEventArgument;

    if (nullptr == Args) {
        return;
    }

    HttpEventArgument = static_cast< struct HttpEventArgument *>(Args);
    Domain = (EPollEventDomain *)HttpEventArgument->EventDomain;
    EventCount = HttpEventArgument->EventCount;
    Server = HttpEventArgument->Server;
    Events = HttpEventArgument->Events;
    Listening = HttpEventArgument->Listening;

    if (Domain == nullptr || Events == nullptr) {
        return;
    }

    for (int i = 0; i<EventCount; i++) {
        S = (Socket *) Events[i].data.ptr;

        if (S == nullptr) {
            continue;
        }

        TempEventArgument = static_cast<struct HttpEventArgument *> (Domain->Allocate(sizeof(HttpEventArgument)));

        if (TempEventArgument == nullptr) {
            continue;
        }

        memset((void *)TempEventArgument, 0, sizeof(HttpEventArgument));

        TempEventArgument->Server = Server;
        TempEventArgument->ArgumentSize = sizeof(HttpEventArgument);
        TempEventArgument->Flags = 0;

        if (S->GetSocketFD() == Listening->GetSocketFD()) {

            ConnectionFd = accept4(Listening->GetSocketFD(), (struct sockaddr *)&SocketAddress, &SocketLength, SOCK_NONBLOCK);

            if ( -1 == ConnectionFd) {
                printf("accept4() failed in HttpEventProcessPromise!\n");
            }

            C = new HttpConnection(ConnectionFd, (struct sockaddr *)&SocketAddress, SocketLength);
            TempEventArgument->Connection = C;
            TempEventArgument->Connected = 1;
            Server->AttachConnection(C);
            Domain->AttachSocket(C, SOCK_READ_WRITE_EVENT);
        }
        else {
            C= (HttpConnection *)S;
            TempEventArgument->Connection = C;
            if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
                TempEventArgument->Read = 1;
            }
            if (Events[i].events & (EPOLLOUT)){
                TempEventArgument->Write = 1;
            }
        }
        TPool->PostPromise(*C->OnEvent, (void *)TempEventArgument);
    }

    Server->EnqueueListening(Listening);
    Domain->Free((void **)&Events);
}

RuntimeError HttpServer::HttpServerEventProcess() {

    Listening *Listen;

    HttpEventArgument *Argument;

    Argument = (HttpEventArgument *)EventDomain.Allocate(sizeof(HttpEventArgument));
    memset(Argument, 0, sizeof(HttpEventArgument));

    Listen = DequeueListening();

    if (Listen == nullptr) {
        Lock.Unlock();
        return RuntimeError(ENOENT, "Can not get A Listening from HttpServer!");
    }

    Lock.Lock();

    Argument->Server = this;
    Argument->Listening = Listen;
    Argument->EventDomain = &EventDomain;
    Argument->ArgumentSize = sizeof(HttpEventArgument);

    Lock.Unlock();

    RuntimeError Error = EventDomain.EventDomainProcess((void *)Argument);

    if (Error.GetErrorCode() == 0) {
        EnqueueListening(Listen);
    }

    return Error;
}