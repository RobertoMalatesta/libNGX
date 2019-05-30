#include "HTTP/HTTPConnection.h"

using namespace ngx::HTTP;

void HTTPConnection::reset() {
    close();
    connectionPool.reset();
}

SocketError HTTPConnection::close() {
    unbindDomain();
    TCPConnection::close();
    return {0};
}

RuntimeError HTTPConnection::bindDomain(EventDomain &D) {

    EventError Error{0};

    if (Domain != nullptr)
        return {EALREADY, "HTTPConnection, bindDomain() connection is already bind"};

    Error = D.attachConnection(static_cast<Connection &>(*this));

    if (Error.GetCode()) {
        return {Error.GetCode(), "HTTPConnection, bindDomain() failed to attachConnection()"};
    }

    Domain = &D;
    return {0};
}

RuntimeError HTTPConnection::unbindDomain() {

    EventError E{0};

    if (Domain != nullptr) {
        E = Domain->detachConnection(static_cast<Connection &>(*this));

        if (E.GetCode()) {
            return {E.GetCode(), "HTTPConnection, unbindDomain() failed to detachConnection()"};
        }

        Domain->stopTimer(TNode);
        Domain = nullptr;
    }
    return {0};
}

void HTTPConnection::HttpConnectionHandle(void *pListen, void *pArg) {
}

//RuntimeError HTTPConnection::HandleDomainEvent(EventType Type) {
//
//    std::lock_guard<spin_lock> g(SocketLock);
//
////    LOG(INFO) << "handle HTTPConnection event, fd: " << SocketFD << ", type: " << EventType;
//
//    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
////        LOG(WARNING) << "connection not attached, fd: " << SocketFD;
//        Close();
//        return {EINVAL, "connection not attached"};
//    }
//
//    Event |= Type;
//
//    if (SocketFD == -1) {
////        LOG(WARNING) << "connection not open, fd: " << SocketFD;
//        Close();
//        return {EFAULT, "connection not open"};
//    };
//
//    return PostJob(EventJob);
//}
//
//void HTTPConnection::OnConnectionEvent(void *PointerToConnection) {
//
//    HTTPConnection *C;
//    RuntimeError Error{0};
//
//    C = static_cast<HTTPConnection *>(PointerToConnection);
//
////    LOG(INFO) << "in connection event, fd: " << C->SocketFD;
//
//    if (C->SocketFD != -1) {
//
//        // Sequence
//        //
//        // 0.   Process Timer
//        //
//        // 1.   Judge request stage
//        // 1.1      Parse request header
//        // 1.2      Filter request(body size, request type, etc)
//        // 1.3      Route request
//        // 1.4      Accept content and deliver it to router(route can reroute the content)
//        // 2.   Filter response and judge if we need to write it to buffer
//        // 3.   Write output buffer and drain the response buffer
//
//        // Stages:
//        //
//        //  HTTP_IDLE
//        //  HTTP_REQUEST_PARSING (fetch header)
//        //  HTTP_REQUEST_PARSED (parse request header)
//        //  HTTP_REQUEST_ROUTED (filter and route)
//        //  HTTP_REQUEST_PROCESS_CONTENT (handle content, input chunk and frames)
//        //  HTTP_REQUEST_POST_FILTER (if we should output it?)
//        //  HTTP_REQUEST_DONE (close connection?)
//
////        LOG(INFO) << "handle connection event, fd: " << C->SocketFD << ", event type: " << Type;
//
//        if ((C->Event & ET_READ) != 0) {
//
//            Error = C->ReadBuffer.ReadConnection(C);
//
//            if (Error.GetCode() == 0) {
//                C->Event &= ~ET_READ;
//            }
//        }
//
//        if ((C->Event & ET_WRITE) != 0) {
//
////            if (Error.GetCode() == 0) {
////                C->Event &= ~ET_WRITE;
////            }
//            // Mark the socket as writable and write all data to it!
//        }
//
//        // Handle Request here
//
////        LOG(INFO) << "writing data to connection, fd: " << C->SocketFD;
////            HTTPParser::ParseHTTPRequest(C->ReadBuffer, C->Request);
//        const u_char Content[] = "HTTP/1.1 200 OK\r\nServer: NGX(TestServer)\nContent-Length: 12\r\n\nHello World!";
//
//        ssize_t size = write(C->GetSocketFD(), Content, sizeof(Content) - 1);
//
//        if (size < 0) {
////            LOG(INFO) << "bad write size < 0, fd: " << C->SocketFD;
//        }
////        LOG(INFO) << "will close socket: " << C->SocketFD;
//        C->Close();
//    } else {
//
////        LOG(INFO) << "connection already closed, skip it`s event" << C->SocketFD;
//
//        if (C->Event & ET_TIMER) {
////            LOG(INFO) << "recycle connection, fd: " << C->SocketFD;
//            C->ParentServer->PutConnection(C);
//        }
//    }
//}


