#include "HTTP/HTTP.h"
#include <fcntl.h>
using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPListening::HTTPListening(SocketAddress &SocketAddress) : TCP4Listening(SocketAddress) {
}

RuntimeError HTTPListening::HandleEventDomain(uint32_t EventType) {

    int TempFD;
    SocketAddress Address{0};
    HTTPConnection *C;

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "Listening is not attached"};
    }

    if ((EventType & ET_READ) != 0) {

        RuntimeError Error{0};

            TempFD = accept4(SocketFD, &Address.sockaddr, &Address.SocketLength, O_NONBLOCK);

            if (TempFD == -1) {
                return {EBADFD, "failed to accept socket"};
            }

            Error = ParentServer->GetConnection(C, TempFD, Address);

            if (Error.GetCode() != 0 || C == nullptr) {
                close(TempFD);
                //[TODO] add warning here!
            }
            C->Lock();
            C->ParentEventDomain->AttachSocket(*C, ET_READ | ET_WRITE);
            C->Unlock();
    }
    return {0};
}

