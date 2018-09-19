#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpConnection::HttpConnection(struct sockaddr *SocketAddress, socklen_t SocketLength):
        TCP4Connection(SocketAddress, SocketLength){
    Lock.Unlock();
    OnConnected = &(HttpConnection::OnHttpConnectionConnected);
    OnRead = &(HttpConnection::OnHttpConnectionRead);
    OnWrite = &(HttpConnection::OnHttpConnectionWrite);
    OnClose = &(HttpConnection::OnHttpConnectionClose);
}

HttpConnection::HttpConnection(int SocketFd, struct sockaddr *SocketAddress, socklen_t SocketLength) :
        TCP4Connection(SocketFd, SocketAddress, SocketLength){
    Lock.Unlock();
    OnConnected = &(HttpConnection::OnHttpConnectionConnected);
    OnRead = &(HttpConnection::OnHttpConnectionRead);
    OnWrite = &(HttpConnection::OnHttpConnectionWrite);
    OnClose = &(HttpConnection::OnHttpConnectionClose);
}

char buffer[2048];

void HttpConnection::OnHttpConnectionConnected(void *Arguments, ngx::Core::ThreadPool *TPool) {
    cout<<"Connected!"<<endl;
    HttpEventPromiseArguments *Args = (HttpEventPromiseArguments *)Arguments;

    HttpServer *S = Args->Server;
    HttpConnection *C = Args->Connection;

    S->EPollDetachSocket(C);
    S->EPollAttachSocket(C);
}

void HttpConnection::OnHttpConnectionRead(void *Arguments, ngx::Core::ThreadPool *TPool) {
    cout<<"Read available!"<<endl;
}

void HttpConnection::OnHttpConnectionWrite(void *Arguments, ngx::Core::ThreadPool *TPool) {
    cout<<"Write avaiable!"<<endl;
}

void HttpConnection::OnHttpConnectionClose(void *Arguments, ngx::Core::ThreadPool *TPool) {
    cout<<"Close called!"<<endl;
}

HttpServer::HttpServer(
        size_t PoolSize,
        int ThreadCount,
        int EPollSize,
        int ConnectionRecycleSize,
        int BufferRecycleSize)
        :EPollEventDomain(PoolSize, ThreadCount, EPollSize, &(HttpEventProcessPromise)),
        Allocator(PoolSize) {}

void HttpServer::HttpEventProcessPromise(void *Args, ThreadPool *TPool) {

    EPollEventProcessArguments *EPollArguments;
    EPollEventDomain *Domain;
    HttpServer *Server;
    epoll_event *Events;
    int EventCount;
    Listening *Listening;
    HttpConnection *C;
    Socket *S;

    int ConnectionFd = -1;
    struct sockaddr_in SocketAddress;
    socklen_t SocketLength;

    if (nullptr == Args) {
        return;
    }

    EPollArguments = static_cast<EPollEventProcessArguments *>(Args);
    Domain = EPollArguments->EventDomain;
    Server = (HttpServer *)(Domain);
    Events = EPollArguments->Events;
    EventCount = EPollArguments->EventCount;
    Listening = EPollArguments->Listening;

    if (Domain == nullptr || Events == nullptr) {
        return;
    }

    Domain->FreeAllocatedMemory((void **)&EPollArguments);

    for (int i = 0; i<EventCount; i++) {
        S = (Socket *) Events[i].data.ptr;

        if (S == nullptr) {
            continue;
        }

        HttpEventPromiseArguments *Argument;

        Argument = (HttpEventPromiseArguments *)Server->Allocator.Allocate(sizeof(HttpEventPromiseArguments));

        if (Argument == nullptr) {
            return;
        }

        Argument->Server = Server;

        if (S->GetSocketFD() == Listening->GetSocketFD()) {
            ConnectionFd = accept4(Listening->GetSocketFD(), (struct sockaddr *)&SocketAddress, &SocketLength, SOCK_NONBLOCK);

            if ( -1 == ConnectionFd) {
                printf("accept4() failed in HttpEventProcessPromise!\n");
            }
            C = new HttpConnection(ConnectionFd, (struct sockaddr *)&SocketAddress, SocketLength);
            Argument->Connection = C;

            TPool->PostPromise(C->OnConnected, (void *)Argument);
            Server->EPollAddConnection(C);
            Server->EPollAttachSocket(C);
        }
        else {
            C = (HttpConnection *)(S);
            Argument->Connection = C;
            if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
                TPool->PostPromise(C->OnRead, (void *)Argument);
            }
            if (Events[i].events & (EPOLLOUT)){
                TPool->PostPromise(C->OnWrite, (void *)Argument);
            }
        }
    }

    Domain->FreeAllocatedMemory((void **)&Events);
    Domain->EPollEnqueueListening(Listening);
}