#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection(struct SocketAddress &SocketAddress, BufferBuilder &BB) :
        Lock(),
        Recyclable(),
        TimerNode(0, HTTPConnection::OnConnectionEvent, nullptr),
        TCP4Connection(SocketAddress) {

    BB.BuildBuffer(ReadBuffer);
    OnEventPromise = HTTPConnection::OnConnectionEvent;
}

HTTPConnection::HTTPConnection(int SocketFd, struct SocketAddress &SocketAddress, BufferBuilder &BB) :
        Lock(),
        Recyclable(),
        TCP4Connection(SocketFd, SocketAddress) {

    BB.BuildBuffer(ReadBuffer);
    OnEventPromise = HTTPConnection::OnConnectionEvent;
}

void HTTPConnection::OnConnectionEvent(void *Arguments, ThreadPool *TPool) {

    EventPromiseArgs *TempArgument;
    EPollEventDomain *EventDomain;
    Socket *TempSocket;
    HTTPConnection *TempConnection;
    EventType Type;

    printf("EnterPromise, Arguments: %p\n", Arguments);

    TempArgument = static_cast<EventPromiseArgs *>(Arguments);

    if (TempArgument->UserArguments[3].Ptr == nullptr ||
        TempArgument->UserArguments[4].Ptr == nullptr ||
        TempArgument->UserArguments[6].Ptr == nullptr) {
        return;
    }

    EventDomain = static_cast<EPollEventDomain *>(TempArgument->UserArguments[4].Ptr);
    TempSocket = static_cast<Socket *>(TempArgument->UserArguments[6].Ptr);
    TempConnection = (HTTPConnection *) TempSocket;
    Type = static_cast<EventType>(TempArgument->UserArguments[7].UInt);

    printf("Event Type: %x, connection fd: %d\n", Type, TempConnection->GetSocketFD());

    if ((Type & ET_CONNECTED) != 0) {
        EventDomain->AttachSocket(TempConnection, SOCK_READ_WRITE_EVENT);
    }
    if ((Type & ET_READ) != 0) {
        Buffer *Buffer = &TempConnection->ReadBuffer;
        Buffer->WriteConnectionToBuffer(TempConnection);
        HTTPParser::ParseHTTPRequest(TempConnection->ReadBuffer, TempConnection->CurrentRequest);
    }
    if ((Type & ET_WRITE) != 0) {
        // Mark the socket as writable and write all data to it!
    }

    printf("LeavePromise, Arguments: %p\n", Arguments);
}

void HTTPConnection::Reset() {
    ReadBuffer.Reset();
}
