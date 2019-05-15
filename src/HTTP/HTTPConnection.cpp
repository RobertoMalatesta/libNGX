#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnection::HTTPConnection() :
        TCPConnection(Address),
        Request(),
        EventJob(HTTPConnection::OnConnectionEvent, this) {
    TimerNode = {0, HTTPConnection::OnTimerEvent, this};
}

RuntimeError HTTPConnection::HandleDomainEvent(EventType Type) {

    LockGuard Guard(&SocketLock);

//    LOG(INFO) << "handle HTTPConnection event, fd: " << SocketFD << ", type: " << EventType;

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
//        LOG(WARNING) << "connection not attached, fd: " << SocketFD;
        Close();
        return {EINVAL, "connection not attached"};
    }

    Event |= Type;

    if (SocketFD == -1) {
//        LOG(WARNING) << "connection not open, fd: " << SocketFD;
        Close();
        return {EFAULT, "connection not open"};
    };

    return PostJob(EventJob);
}

void HTTPConnection::OnTimerEvent(void *PointerToConnection) {

    HTTPConnection *C;

    C = static_cast<HTTPConnection *>(PointerToConnection);

    C->Event |= ET_TIMER;

//    LOG(INFO) << "in timer func, fd: " << C->SocketFD;

    HTTPConnection::OnConnectionEvent(PointerToConnection);
}

void HTTPConnection::OnConnectionEvent(void *PointerToConnection) {

    HTTPConnection *C;
    RuntimeError Error{0};

    C = static_cast<HTTPConnection *>(PointerToConnection);

//    LOG(INFO) << "in connection event, fd: " << C->SocketFD;

    if (C->SocketFD != -1) {

        // Sequence
        //
        // 0.   Process Timer
        //
        // 1.   Judge request stage
        // 1.1      Parse request header
        // 1.2      Filter request(body size, request type, etc)
        // 1.3      Route request
        // 1.4      Accept content and deliver it to router(route can reroute the content)
        // 2.   Filter response and judge if we need to write it to buffer
        // 3.   Write output buffer and drain the response buffer

        // Stages:
        //
        //  HTTP_IDLE
        //  HTTP_REQUEST_PARSING (fetch header)
        //  HTTP_REQUEST_PARSED (parse request header)
        //  HTTP_REQUEST_ROUTED (filter and route)
        //  HTTP_REQUEST_PROCESS_CONTENT (handle content, input chunk and frames)
        //  HTTP_REQUEST_POST_FILTER (if we should output it?)
        //  HTTP_REQUEST_DONE (close connection?)

//        LOG(INFO) << "handle connection event, fd: " << C->SocketFD << ", event type: " << Type;

        if ((C->Event & ET_READ) != 0) {

            Error = C->ReadBuffer.ReadConnection(C);

            if (Error.GetCode() == 0) {
                C->Event &= ~ET_READ;
            }
        }

        if ((C->Event & ET_WRITE) != 0) {

//            if (Error.GetCode() == 0) {
//                C->Event &= ~ET_WRITE;
//            }
            // Mark the socket as writable and write all data to it!
        }

        // Handle Request here

//        LOG(INFO) << "writing data to connection, fd: " << C->SocketFD;
//            HTTPParser::ParseHTTPRequest(C->ReadBuffer, C->Request);
        const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";

        ssize_t size = write(C->GetSocketFD(), Content, sizeof(Content) - 1);

        if (size < 0) {
//            LOG(INFO) << "bad write size < 0, fd: " << C->SocketFD;
        }
//        LOG(INFO) << "will close socket: " << C->SocketFD;
        C->Close();
    } else {

//        LOG(INFO) << "connection already closed, skip it`s event" << C->SocketFD;

        if (C->Event & ET_TIMER) {
//            LOG(INFO) << "recycle connection, fd: " << C->SocketFD;
            C->ParentServer->PutConnection(C);
        }
    }
}

void HTTPConnection::Reset() {

    // Clear all attached events
    AttachedEvent = Event = 0;

    // Reset read buffer
    ReadBuffer.Reset();

    // Reset memory pool
    MemPool.reset();

    // Reset timer node
    TimerNode.Reset();
}

SocketError HTTPConnection::Close() {

    // Reset Request, Response, BufferIn and BufferOut

    // Detach read write event
    ParentEventDomain->DetachSocket(*this, ET_READ | ET_WRITE);

    // Close TCP Connection
    TCPConnection::Close();

    // Set a timer to put Connection to collector
    ParentEventDomain->SetTimer(*this, CONNECTION_RECYCLE_WAIT_TIME, TM_ONCE);

    return {0};
}
