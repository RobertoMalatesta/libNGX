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
            printf("failed to accept()\n");
            //[TODO] add warning here!
            return {errno, "failed to accept socket"};
        } else {

            printf("accept() new connection: %d\n", NewFD);
            Error = ParentServer->GetConnection(C, NewFD, Address);

            Error.PrintError();

            if (Error.GetCode() != 0 || C == nullptr) {
                close(NewFD);
            }

            Error.PrintError();

            C->ParentEventDomain->AttachSocket(*C, ET_READ | ET_WRITE).PrintError();
        }
    }

    return {0};
}

