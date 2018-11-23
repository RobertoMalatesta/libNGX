#include "HTTP/HTTP.h"
#include <fcntl.h>
using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPListening::HTTPListening(SocketAddress &SocketAddress) : TCP4Listening(SocketAddress) {
}

RuntimeError HTTPListening::HandleEventDomain(uint32_t EventType) {

    int NewFD;
    SocketAddress Address{0};
    HTTPConnection *C;

    LockGuard Lock(&SocketLock);

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "Listening is not attached"};
    }

    if ((EventType & ET_READ) != 0) {

        RuntimeError Error{0};

        NewFD = accept4(SocketFD, &Address.sockaddr, &Address.SocketLength, O_NONBLOCK);

        if (NewFD == -1) {
            LOG(WARNING) << "failed to accept, errno: " << errno;
            return {errno, "failed to accept socket"};
        } else {

            LOG(INFO) << "accept() new connection, fd: " << NewFD;
            Error = ParentServer->GetConnection(C, NewFD, Address);

            LOG(INFO) << "get connection";

            if (Error.GetCode() != 0 || C == nullptr) {
                LOG(INFO) << "get connection object failed, will close connection, error: " << Error.GetError();
                close(NewFD);
            } else {
                LOG(INFO) << "attach connection to event domain, fd: "<< NewFD << ", error: " << C->ParentEventDomain->AttachSocket(*C, ET_READ | ET_WRITE).GetError();
            }
        }
    }

    return {0};
}

