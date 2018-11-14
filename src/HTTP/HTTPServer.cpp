#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPServer::HTTPServer(
        size_t PoolSize,
        int ThreadCount,
        int EPollSize,
        size_t BufferBlockSize,
        uint64_t ConnectionRecycleSize,
        uint64_t BufferRecycleSize) :
        Server(),
        ConnectionBuilder(BufferBlockSize, BufferRecycleSize, ConnectionRecycleSize),
        EventDomain(PoolSize, ThreadCount, EPollSize) {}

RuntimeError HTTPServer::PostProcessFinished(EventPromiseArgs &Arguments) {

    HTTPServer *TempServer;
    TCP4Listening *TempListen;

    TempServer = static_cast<HTTPServer *>(Arguments.UserArguments[3].Ptr);
    TempListen = static_cast<TCP4Listening *>(Arguments.UserArguments[5].Ptr);
    TempServer->EnqueueListening(TempListen);

    return {0};
}

RuntimeError HTTPServer::PostConnectionEvent(EventPromiseArgs &Argument) {

    EventType TargetType;
    Listening *TargetListening;
    HTTPConnection *TargetConnection;
    SocketAddress *TargetSocketAddress;
    EPollEventDomain *TargetEventDomain;

    TargetConnection = static_cast<HTTPConnection *>(Argument.UserArguments[6].Ptr);
    TargetType = Argument.UserArguments[7].UInt;

    if ((TargetType & ET_CONNECTED) != 0) {

        int SocketFD = Argument.UserArguments[0].UInt;

        TargetListening = static_cast<Listening *>(Argument.UserArguments[5].Ptr);
        TargetEventDomain = static_cast<EPollEventDomain *>(Argument.UserArguments[4].Ptr);
        TargetSocketAddress = static_cast<SocketAddress *>(Argument.UserArguments[1].Ptr);

        if (ConnectionBuilder.Get(TargetConnection, SocketFD, TargetSocketAddress, this, TargetListening,
                              TargetEventDomain) == 0) {
            AttachConnection(TargetConnection);
        }
        else {
            close(SocketFD);
           // TODO: handle connection pool drain, add special response
        }
    }
    TargetConnection->Lock.Lock();
    TargetConnection->Event = TargetType;

    if (TargetConnection->Open == 1) {
        EventDomain.PostPromise(*TargetConnection->OnEventPromise, static_cast<void *>(TargetConnection));
    }

    return {0, nullptr};
}

RuntimeError HTTPServer::HTTPServerEventProcess() {

    Listening *Listen;
    EventPromiseArgs Argument = {nullptr};

    memset(&Argument, 0, sizeof(EventPromiseArgs));

    Listen = DequeueListening();

    if (Listen == nullptr) {
        return {ENOENT, "Can not get a Listening from HTTPServer"};
    }

    Argument.UserArguments[3].Ptr = static_cast<void *>(this);
    Argument.UserArguments[4].Ptr = static_cast<void *>(&EventDomain);
    Argument.UserArguments[5].Ptr = (void *) Listen;

    return EventDomain.EventDomainProcess(Argument);
}

RuntimeError HTTPServer::PutConnection(HTTPConnection *&C) {

    DetachConnection(C);
    C->Reset();
    ConnectionBuilder.Put(C);

    return {0};
}
