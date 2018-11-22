#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection() :
        TCP4Connection(Address),
        Request(nullptr) {
    TimerNode.Callback = HTTPConnection::OnTimerEvent;
    TimerNode.Argument = this;

    Type = SOCK_TYPE_STREAM, Version = 4, IsListen = 1;
}

RuntimeError HTTPConnection::SetSocketAddress(int SocketFD, struct SocketAddress &Address) {

    LockGuard Guard(&SocketLock);
    this->SocketFD = SocketFD;
    this->Address = Address;

    return {0};
}

RuntimeError HTTPConnection::HandleEventDomain(uint32_t EventType) {

    LockGuard Guard(&SocketLock);

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "connection not attached"};
    }

    Event = EventType;

    if (Open == 0) {
        SocketFD = -1;
        return {EFAULT, "connection not open"};
    };

    return ParentEventDomain->PostPromise(HTTPConnection::OnConnectionEvent, static_cast<void *>(this));
}

void HTTPConnection::OnTimerEvent(void *PointerToConnection, ThreadPool *TPool) {

    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->Lock();
    C->Event |= ET_TIMER;

    if (C->Open == 0) {
        C->ParentServer->PutConnection(C);
    }
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *) {

    EventType Type;
    HTTPConnection *C;

//    printf("Enter Promise, PointerToConnection: %p\n", PointerToConnection);

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->Lock();
    if (C->Open == 1) {
        Type = C->Event;
//            printf("Event Type: %x, connection fd: %d\n", Type, C->GetSocketFD());

        if ((Type & ET_READ) != 0) {
            C->ReadBuffer.ReadFromConnection(C);
        }

        if ((Type & ET_WRITE) != 0) {
            // Mark the socket as writable and write all data to it!
        }

        // Handle Request here

        if ((Type & ET_READ) != 0) {
//            HTTPParser::ParseHTTPRequest(C->ReadBuffer, C->Request);
            if (C->Open == 1) {

                const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";

                ssize_t size = write(C->GetSocketFD(), Content, sizeof(Content) - 1);

                if (size <= 0) {
//                        printf("bad write!\n");
                }
            }
        }
        if ((Type & ET_READ) != 0) {
            C->Close();
        } else {
        }
    } else {
//            printf("connection closed, skip this event!\n");
    }

    C->SocketLock.Unlock();
//    printf("Leave Promise, PointerToConnection: %p\n", PointerToConnection);
}

void HTTPConnection::Reset() {
    ReadBuffer.Reset();
    MemPool.Reset();
    TimerNode.Reset();
}

SocketError HTTPConnection::Close() {

    ParentEventDomain->DetachSocket(*this, ET_READ | ET_WRITE);
    if (SocketFD != -1 || Open == 1) {

        if (close(SocketFD) == -1) {
            printf("failed to close socket!\n");
        }
        SocketFD = -1, Open = 0;
    }

    ParentEventDomain->SetTimer(*this, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);
    return {0};
}
