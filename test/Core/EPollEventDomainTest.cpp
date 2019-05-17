#include "Core/Core.h"

using namespace ngx::Core;
using namespace ngx::Core::Target::Linux;

static void HTTPEventProcessJob(void *Args, ThreadPool *TPool) {

//    EPollEventDomainArgument *EPollArguments;
//
//    if (nullptr == Args) {
//        return;
//    }
//
//    EPollArguments = static_cast<EPollEventDomainArgument *>(Args);
//    EPollEventDomain *Domain = static_cast<EPollEventDomain *>(EPollArguments->EventDomain);
//    epoll_event *Events = EPollArguments->Events;
//    Listening *Listening = EPollArguments->Listening;
//
//    if (Domain == nullptr || Events == nullptr) {
//        return;
//    }

//    Domain->FreeAllocatedMemory((void **)&EPollArguments);
//    Domain->FreeAllocatedMemory((void **)&Events);
//    Domain->EPollEnqueueListening(Listening);
}

int EPollEventDomainTest() {

    struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(8080),
            .sin_addr = {
                    .s_addr = INADDR_ANY
            }
    };

    Address_t Address(addr);

    TCPListening Listen(Address);
    EPollEventDomain Domain(4, 31723);

    Listen.SetPortReuse(true).PrintError();
    Listen.Listen().PrintError();

//    Domain.EPollEnqueueListening(&Listen).PrintError();

    int Count = 500000;

    while (Count-- > 0) {
//        RuntimeError Error = Domain.EventDomainProcess(nullptr);

//        if (Error.GetCode() != 0) {
//            Error.PrintError();
//        }
        ForceSleep(NANO_SECOND_SIZE / 2000);
    }

    return 0;
}