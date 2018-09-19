#include "Core/Core.h"

using namespace ngx::Core;

static void HttpEventProcessPromise(void *Args, ThreadPool *TPool) {

    EPollEventProcessArguments *EPollArguments;

    if (nullptr == Args) {
        return;
    }

    EPollArguments = static_cast<EPollEventProcessArguments *>(Args);
    EPollEventDomain *Domain = EPollArguments->EventDomain;
    epoll_event *Events = EPollArguments->Events;
    Listening *Listening = EPollArguments->Listening;

    if (Domain == nullptr || Events == nullptr) {
        return;
    }

    Domain->FreeAllocatedMemory((void **)&EPollArguments);
    Domain->FreeAllocatedMemory((void **)&Events);
    Domain->EPollEnqueueListening(Listening);
}

int EPollEventDomainTest() {

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(8080);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    TCP4Listening Listen((sockaddr *)&server_sockaddr, sizeof(server_sockaddr));
    EPollEventDomain Domain(40960, 4, 31723, &HttpEventProcessPromise);

    Listen.SetPortReuse(true).PrintError();
    Listen.Listen().PrintError();

    Domain.EPollEnqueueListening(&Listen).PrintError();

    int Count = 500000;

    while (Count -- > 0) {
        RuntimeError Error = Domain.EventDomainProcess();

        if (Error.GetErrorCode() != 0) {
            Error.PrintError();
        }

        ForceUSleep(500);
    }

    return 0;
}