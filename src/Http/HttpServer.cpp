#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpServer::HttpServer(
        size_t PoolSize,
        int ThreadCount,
        int EPollSize,
        size_t BufferBlockSize,
        uint64_t ConnectionRecycleSize,
        uint64_t BufferRecycleSize) :
        Server(),
        ConnectionRecyclers(BufferBlockSize, BufferRecycleSize, ConnectionRecycleSize),
        EventDomain(PoolSize, ThreadCount, EPollSize) {}

RuntimeError HttpServer::PostProcessFinished(EventPromiseArgs *Arguments) {

    HttpServer *TempServer;
    TCP4Listening *TempListen;

    EventDomain.Free((void **) &Arguments);

    if (nullptr == Arguments
        || nullptr == Arguments->UserArguments[3].Ptr
        || nullptr == Arguments->UserArguments[5].Ptr
            ) {

        return RuntimeError(EINVAL);
    }

    TempServer = static_cast<HttpServer *>(Arguments->UserArguments[3].Ptr);
    TempListen = static_cast<TCP4Listening *>(Arguments->UserArguments[5].Ptr);
    TempServer->EnqueueListening(TempListen);

    return RuntimeError(0);
}

RuntimeError HttpServer::PostConnectionEvent(EventPromiseArgs *Arguments) {

    int SocketFd;
    void *TempPointer;
    Socket *TempSocket;
    HttpConnection *TempConnection;
    SocketAddress *SockAddr;
    EventType Type;

    Type = static_cast<EventType>(Arguments->UserArguments[7].UInt);

    if ((Type & ET_CONNECTED) != 0) {

        if (nullptr == Arguments->UserArguments[1].Ptr) {
            return RuntimeError(EINVAL);
        }

        SocketFd = Arguments->UserArguments[0].UInt;
        SockAddr = static_cast<SocketAddress *>(Arguments->UserArguments[1].Ptr);
        TempConnection = ConnectionRecyclers.Get(SocketFd, *SockAddr);
        TempSocket = static_cast<Socket *>(TempConnection);

        EventDomain.Free((void **) &SockAddr);

        Arguments->UserArguments[6].Ptr = (void *) TempSocket;
        AttachConnection(TempConnection);
    } else {

        TempPointer = Arguments->UserArguments[6].Ptr;

        if (nullptr == TempPointer) {
            return RuntimeError(EINVAL);
        }
        TempSocket = static_cast<Socket *>(TempPointer);
        TempConnection = (HttpConnection *) TempSocket;
    }

    EventDomain.PostPromise(TempConnection->OnEventPromise, Arguments);
    return RuntimeError(0);
}

RuntimeError HttpServer::HttpServerEventProcess() {

    Listening *Listen;
    EventPromiseArgs Arguments = {nullptr};

    memset(&Arguments, 0, sizeof(EventPromiseArgs));

    Listen = DequeueListening();

    if (Listen == nullptr) {
        return RuntimeError(ENOENT, "Can not get A Listening from HttpServer!");
    }

    Arguments.UserArguments[3].Ptr = (void *) this;
    Arguments.UserArguments[4].Ptr = (void *) &EventDomain;
    Arguments.UserArguments[5].Ptr = (void *) Listen;

    RuntimeError Error = EventDomain.EventDomainProcess(&Arguments);

    if (Error.GetErrorCode() == 0) {
        EnqueueListening(Listen);
    }

    return Error;
}

