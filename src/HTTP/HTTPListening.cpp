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

        while(true) {

            NewFD = accept(SocketFD, &Address.sockaddr, &Address.SocketLength);

            if (NewFD == -1) {
                //[TODO] add warning here!

                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    break;
                }

                return {errno, "failed to accept socket"};
            }

            Error = ParentServer->GetConnection(C, NewFD, Address);

            if (Error.GetCode() != 0 || C == nullptr) {
                close(NewFD);
            }

            C->ParentEventDomain->AttachSocket(*C, ET_READ | ET_WRITE);
        }

    }

    return {0};
}

