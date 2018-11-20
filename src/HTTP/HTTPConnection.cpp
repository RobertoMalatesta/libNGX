#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPConnection::HTTPConnection() :
        TCP4Connection(Address),
        Request(nullptr) {
    TimerNode.Callback = HTTPConnection::OnTimerEventWarp;
    TimerNode.Argument = this;
}

RuntimeError HTTPConnection::SetSocketAddress(int SocketFD, struct SocketAddress &Address) {
    Lock();
    this->SocketFD = SocketFD;
    this->Address = Address;
    Unlock();

    return {0};
}

RuntimeError HTTPConnection::HandleEventDomain(uint32_t EventType) {

    Lock();

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        Unlock();
        return {EINVAL, "connection not attached"};
    }

    Event = EventType;

    if (Open == 0) {
        SocketFD = -1;
        Unlock();
        return {EFAULT, "connection not open"};
    };

    Unlock();
    return ParentEventDomain->PostPromise(HTTPConnection::OnConnectionEvent, static_cast<void *>(this));
}

void HTTPConnection::OnTimerEventWarp(void *PointerToConnection, ThreadPool *TPool) {

    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);
    C->Lock();
    C->Event |= ET_TIMER;
    C->Unlock();

    if (C->Open == 1) {
        OnConnectionEvent(PointerToConnection, TPool);
    } else {

        printf("recycle connection: %p\n", C);
        if (C->ParentServer != nullptr) {
            C->ParentServer->PutConnection(C);
        }
    }
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection, ThreadPool *) {

    EventType Type;
    HTTPConnection *C;

    printf("Enter Promise, PointerToConnection: %p\n", PointerToConnection);

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->Lock();

    if (C->Open == 1) {
        Type = C->Event;
        printf("Event Type: %x, connection fd: %d\n", Type, C->GetSocketFD());

        if ((Type & ET_READ) != 0) {
            C->ReadBuffer.ReadFromConnection(C);
        }

        if ((Type & ET_WRITE) != 0) {
            // Mark the socket as writable and write all data to it!
        }

        if ((Type & ET_TIMER) != 0) {
            printf("In timer event process: %p\n", C);
            // Handle timer
        }

        C->Unlock();

        // Handle Request here

        C->Lock();
        if ((Type & ET_READ) != 0) {
//            HTTPParser::ParseHTTPRequest(C->ReadBuffer, C->Request);
            if (C->Open == 1) {

                const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";

                ssize_t size = write(C->GetSocketFD(), Content, sizeof(Content) - 1);

                if (size <= 0) {
                    printf("bad write!\n");
                }
            }
        }
        C->Unlock();
        if ((Type & ET_READ) != 0) {
            C->Close();
        }
    } else {
        printf("connection closed, skip this event!\n");
        C->Unlock();
    }

    printf("Leave Promise, PointerToConnection: %p\n", PointerToConnection);
}

void HTTPConnection::Reset() {

    if (ParentEventDomain != nullptr) {
        ParentEventDomain->SetTimer(*this, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);
    }

    Lock();
    ParentEventDomain = nullptr;
    ParentServer = nullptr;
    ReadBuffer.Reset();
    MemPool.Reset();
    Unlock();
}

SocketError HTTPConnection::Close() {

    ParentEventDomain->DetachSocket(*this, ET_READ | ET_WRITE);

    Lock();
    if (SocketFD != -1 || Open == 1) {

        if (close(SocketFD) == -1) {
            printf("failed to close socket!\n");
        }
        SocketFD = -1, Open = 0;
    }
    Unlock();

    ParentEventDomain->SetTimer(*this, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);
    return {0};
}
