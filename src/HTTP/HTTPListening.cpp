#include "HTTP/HTTP.h"
#include <fcntl.h>

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPListening::HTTPListening(SocketAddress &Address) : TCPListening(Address) {
}

RuntimeError HTTPListening::HandleDomainEvent(EventType Type) {

    int NewFD;
    RuntimeError Error{0};

    HTTPConnection *C;
    SocketAddress Address;
    socklen_t SocketLength = sizeof(Address);

    LockGuard Lock(&SocketLock);

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "Listening is not attached"};
    }

    if ((Type & ET_READ) != 0) {


        NewFD = accept4(SocketFD, &Address.sockaddr, &SocketLength, O_NONBLOCK);

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
                close(NewFD);
            } else {
//                LOG(INFO) << "attach connection to event domain, fd: " << NewFD << ", error: "
                C->ParentEventDomain->AttachSocket(*C, ET_READ | ET_WRITE).GetError();
            }
        }
    }

    return {0};
}

