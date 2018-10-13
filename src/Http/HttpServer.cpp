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
}

RuntimeError HttpServer::PostProcessFinished(EventPromiseArgs *Arguments) {
//    Server->EnqueueListening(Listening);
//    EventDomain->Free((void **)&Events);
    EventDomain.Free((void **) &Arguments);
    return RuntimeError(0);
}

RuntimeError HttpServer::PostNewConnection(EventPromiseArgs *Arguments) {

    int SocketFd;
    SocketAddress *SockAddr;
    socklen_t SocketLength;
    HttpConnection *Connection;

    if (nullptr == Arguments->UserArguments[1].Ptr) {
        return RuntimeError(EINVAL);
    }

    SocketFd = Arguments->UserArguments[0].UInt;
    SockAddr = static_cast<SocketAddress *>(Arguments->UserArguments[1].Ptr);
    SocketLength = Arguments->UserArguments->UInt;
    Connection = new HttpConnection(SocketFd, &SockAddr->sockaddr, SocketLength);
    Arguments->UserArguments[6].Ptr = (void *)Connection;

    AttachConnection(Connection);
    EventDomain.PostPromise(Connection->OnConnected, Arguments);

    return RuntimeError(0);
}

RuntimeError HttpServer::PostConnectionRead(EventPromiseArgs *Arguments) {

    void *TempPointer;
    HttpConnection *Connection;

    TempPointer = Arguments->UserArguments[6].Ptr;

    if (nullptr == TempPointer) {
        return RuntimeError(EINVAL);
    }

    Connection = static_cast<HttpConnection *>(TempPointer);
    printf("post read promise\n");
    EventDomain.PostPromise(Connection->OnRead, Arguments);
    return RuntimeError(0);
}

RuntimeError HttpServer::PostConnectionWrite(EventPromiseArgs *Arguments) {

    void *TempPointer;
    HttpConnection *Connection;

    TempPointer = Arguments->UserArguments[6].Ptr;

    if (nullptr == TempPointer) {
        return RuntimeError(EINVAL);
    }

    Connection = static_cast<HttpConnection *>(TempPointer);

    printf("Post write promise: %p\n", Arguments);
    EventDomain.PostPromise(Connection->OnWrite, Arguments);

    return RuntimeError(0);
}

RuntimeError HttpServer::PostConnectionClosed(EventPromiseArgs *Arguments) {

    void *TempPointer;
    HttpConnection *Connection;

    TempPointer = Arguments->UserArguments[6].Ptr;

    if (nullptr == TempPointer) {
        return RuntimeError(EINVAL);
    }

    Connection = static_cast<HttpConnection *>(TempPointer);
    printf("post closed promise\n");
    EventDomain.PostPromise(Connection->OnClosed, Arguments);
    return RuntimeError(0);
}

RuntimeError HttpServer::HttpServerEventProcess() {

    Listening *Listen;
    EventPromiseArgs Arguments = {0};

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

