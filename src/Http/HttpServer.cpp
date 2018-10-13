#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpServer::HttpServer(
        size_t PoolSize,
        int ThreadCount,
        int EPollSize,
        int ConnectionRecycleSize,
        int BufferRecycleSize) : Server(),
        EventDomain(PoolSize, ThreadCount, EPollSize) {
    ProcessFinished = &HttpServer::HttpOnEventProcessFinished;
    NewConnection = &HttpServer::HttpOnNewConnection;
    ConnectionRead = &HttpServer::HttpOnConnectionRead;
    ConnectionWrite = &HttpServer::HttpOnConnectionWrite;
}

RuntimeError HttpServer::PostProcessFinished(void *Arguments) {
    return RuntimeError(0);
}

RuntimeError HttpServer::PostNewConnection(void *Arguments) {
    return RuntimeError(0);
}

RuntimeError HttpServer::PostConnectionRead(void *Arguments) {
    return RuntimeError(0);
}

RuntimeError HttpServer::PostConnectionWrite(void *Arguments) {
    return RuntimeError(0);
}


void HttpServer::HttpEventProcessPromise(void *Args, ThreadPool *TPool) {

//    void *TempPointer;
//    Socket *S;
//    HttpConnection *C;
//    HttpServer *Server;
//    epoll_event *Events;
//    Listening *Listening;
//    socklen_t SocketLength;
//    EPollEventDomain *EventDomain;
//    struct sockaddr_in SocketAddress;
//    int EventCount, ConnectionFd;
//    EPollEventDomainArgument *EventArgument, *TempArgument;
//
//    if (nullptr == Args) {
//        return;
//    }
//
//    EventArgument = static_cast<EPollEventDomainArgument *>(Args);
//    Events = (epoll_event *)EventArgument->UserArguments[0].Ptr;
//
//    Listening = EventArgument->Listening;
//    EventCount = EventArgument->EventCount;
//    EventDomain = EventArgument->EventDomain;
//    Server = static_cast<HttpServer *>(EventArgument->UserArguments[0].Ptr);
//
//    if (EventDomain == nullptr || Events == nullptr) {
//        return;
//    }
//
//    for (int i = 0; i<EventCount; i++) {
//        S = (Socket *) Events[i].data.ptr;
//
//        if (S == nullptr) {
//            continue;
//        }
//
//        TempPointer = EventDomain->Allocate((sizeof(EPollEventDomainArgument)));
//
//        if (TempPointer == nullptr) {
//            continue;
//        }
//
//        TempArgument = static_cast<EPollEventDomainArgument *>(TempPointer);
//
//        memcpy(TempArgument, EventArgument, sizeof(EPollEventDomainArgument));
//        TempArgument = static_cast<EPollEventDomainArgument *>(EventDomain->Allocate(sizeof(EPollEventDomainArgument)));
//        TempArgument->UserArguments[4].Ptr = Server;
//        TempArgument->UserArguments[6].UInt = 0;
//        TempArgument->EventDomain = EventDomain;
//
//        if (S->GetSocketFD() == Listening->GetSocketFD()) {
//
//            ConnectionFd = accept4(Listening->GetSocketFD(), (struct sockaddr *)&SocketAddress, &SocketLength, SOCK_NONBLOCK);
//
//            if ( -1 == ConnectionFd) {
//                printf("accept4() failed in HttpEventProcessPromise!\n");
//            }
//
//            C = new HttpConnection(ConnectionFd, (struct sockaddr *)&SocketAddress, SocketLength);
//            TempArgument->UserArguments[5].Ptr = C;
//            TempArgument->UserArguments[6].UInt |= ET_CONNECTED;
//        }
//        else {
//            C= (HttpConnection *)S;
//            TempArgument->UserArguments[5].Ptr = C;
//            if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
//                TempArgument->UserArguments[6].UInt |= ET_READ;
//            }
//            if (Events[i].events & (EPOLLOUT)){
//                TempArgument->UserArguments[6].UInt |= ET_WRITE;
//            }
//        }
//        TPool->PostPromise(*C->OnEvent, (void *)TempArgument);
//    }
//
//    Server->EnqueueListening(Listening);
//    EventDomain->Free((void **)&Events);
}

RuntimeError HttpServer::HttpServerEventProcess() {

    Listening *Listen;
    EventPromiseArgs Arguments;

    memset(&Arguments, 0, sizeof(EventPromiseArgs));

    Listen = DequeueListening();

    if (Listen == nullptr) {
        return RuntimeError(ENOENT, "Can not get A Listening from HttpServer!");
    }

    Arguments.UserArguments[3].Ptr = (void *)this;
    Arguments.UserArguments[4].Ptr = (void *)&EventDomain;
    Arguments.UserArguments[5].Ptr = (void *)Listen;

    RuntimeError Error = EventDomain.EventDomainProcess(&Arguments);

    if (Error.GetErrorCode() == 0) {
        EnqueueListening(Listen);
    }

    return Error;
}


void HttpServer::HttpOnEventProcessFinished(void *Args, ThreadPool *TPool) {

}
void HttpServer::HttpOnNewConnection(void *Args, ThreadPool *TPool) {

}
void HttpServer::HttpOnConnectionRead(void *Args, ThreadPool *TPool) {

}
void HttpServer::HttpOnConnectionWrite(void *Args, ThreadPool *TPool) {

}
