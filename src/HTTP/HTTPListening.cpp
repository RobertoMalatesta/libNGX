#include "HTTP/HTTP.h"

using namespace ngx::Core;
using namespace ngx::HTTP;

HTTPListening::HTTPListening(SocketAddress &SocketAddress) : TCP4Listening(SocketAddress) {
}

RuntimeError HTTPListening::HandleEventDomain(uint32_t EventType) {

    int TempFD = -1;
    SocketAddress Address{0};
    HTTPConnection *C;

    if (ParentServer == nullptr || ParentEventDomain == nullptr) {
        return {EINVAL, "Listening is not attached"};
    }

    if ((EventType & ET_READ) != 0) {

        RuntimeError Error{0};

        TempFD = accept4(SocketFD, &Address.sockaddr, &Address.SocketLength, SOCK_NONBLOCK);

        if (TempFD == -1) {
            return {EFAULT, "failed to accept socket"};
        }

        Error = ParentServer->GetConnection(C, TempFD, Address);

        if (Error.GetCode() != 0) {
            close(TempFD);
            return Error;
        }

        ParentServer->AttachConnection(C);
    }
    return {0};
}

