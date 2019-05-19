#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

RuntimeError HTTPListen::bindDomain(EventDomain &D) {
    EventError E{0};

    if(Domain!= nullptr)
        return {EALREADY, "HTTPListen, bindDomain() already bind"};

    E = D.attachListen(static_cast<Listen &>(*this));

    if (E.GetCode()) {
        return {E.GetCode(), "HTTPListen, bindDomain() failed to detachConnection()"};
    }

    Domain = &D;
    return {0};
}

RuntimeError HTTPListen::unbindDomain() {
    EventError E{0};

    if (Domain != nullptr) {
        E = Domain->detachListen(static_cast<Listen &>(*this));

        if (E.GetCode()) {
            return {E.GetCode(), "HTTPListen, unbindDomain() failed to detachListen()"};
        }

        Domain->stopTimer(TNode);
        Domain = nullptr;
    }
    return {0};
}

void HTTPListen::HttpListenHandle(void *pListen, void *Event) {

    Event_t E;
    HTTPListen *Listen;

    E = static_cast<Event_t >((long)Event);
    Listen = static_cast<HTTPListen *>(pListen);

}

RuntimeError HTTPListen::HandleDomainEvent(EventType Type) {

    int NewFD;
    RuntimeError Error{0};

    HTTPConnection *C;
    Address_t Address;
    socklen_t SocketLength = sizeof(Address);

    std::lock_guard<spin_lock> g(SocketLock);

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "Listening is not attached"};
    }

    if ((Type & ET_READ) != 0) {


        NewFD = accept4(SocketFD, &Address.Addr4, &SocketLength, O_NONBLOCK);

        if (NewFD == -1) {
//            LOG(WARNING) << "failed to accept, errno: " << errno;
            return {errno, "failed to accept socket"};
        } else {

//            LOG(INFO) << "accept() new connection, fd: " << NewFD;
            Error = ParentServer->GetConnection(C, NewFD, Address);

//            LOG(INFO) << "get connection";

            if (Error.GetCode() != 0 || C == nullptr) {
//                can not create connection to handle it
//                LOG(INFO) << "get connection object failed, will close connection, error: " << Error.GetError();
                ::close(NewFD);
            } else {
//                LOG(INFO) << "attach connection to event domain, fd: " << NewFD << ", error: "
                C->ParentEventDomain->AttachSocket(*C, ET_READ | ET_WRITE).GetError();
            }
        }
    }

    return {0};
}
