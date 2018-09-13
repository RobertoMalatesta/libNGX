#include "Core/Core.h"

#include <sys/epoll.h>

using namespace ngx::Core;

typedef struct _EPollEventProcessArguments{
    EPollEventDomain *EventDomain;
    Listening *PointerToListening;
} EPollEventProcessArguments;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize): EventDomain(PoolSize, ThreadCount){
    EPollFD = epoll_create(EPollSize);
}

EPollEventDomain::~EPollEventDomain() {
}

EventError EPollEventDomain::EPollAttachConnection(Connection *C)  {

    struct epoll_event Event;
    int ConnectionFD = C->GetFD();

    Event.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
    Event.data.ptr = (void *)C;

    if (-1 == EPollFD || -1 == ConnectionFD) {
        return EventError(-1, "Epoll initial failed!");
    }

    if (-1 == epoll_ctl( EPollFD, EPOLL_CTL_ADD, ConnectionFD, &Event)) {
        return EventError(errno, "Failed to attach connection to epoll!");
    }

    return EventError(0);
}

EventError EPollEventDomain::EPollDetachConnection(Connection *C)  {
    return EventError(0);
}

EventError EPollEventDomain::EPollPostListenPromise(Listening *Listening) {

    if (-1==EPollFD ) {
        return EventError(ENOENT, "EPollEventDomain initial failed!");
    }

    if (nullptr==Listening || -1 == Listening->GetSocketFD()) {
        return EventError(EINVAL, "Invalid input socket!");
    }

    if (Waiting.test_and_set()) {
        return EventError(EALREADY, "EPollEventDomain is already waiting for events!");
    }

    EPollEventProcessArguments * Arguments =
            static_cast<EPollEventProcessArguments *>(Allocator.Allocate(sizeof(EPollEventProcessArguments)));

    if (nullptr == Arguments) {
        return EventError(ENOMEM, "No sufficent memory!");
    }

    Arguments->EventDomain = this;
    Arguments->PointerToListening = Listening;


    TPool.PostPromise(&(EPollEventDomain::EPollEventProcessPromise), Arguments);

    return EventError(0);
}

RuntimeError EPollEventDomain::EventDomainProcess() {
    return RuntimeError(0);
}

void EPollEventDomain::EPollEventProcessPromise(void *Args, ThreadPool *TPool) {

    if (nullptr == Args) {
        return;
    }

    EPollEventProcessArguments *EPollArguments = static_cast<EPollEventProcessArguments *>(Args);

    EPollEventDomain *Domain = EPollArguments->EventDomain;
    Listening *Listening = EPollArguments->PointerToListening;

    printf("PointerToEventDomain: %p,PointerToListening: %p,PointerToThreadPool: %p",Domain, Listening, TPool);

    Domain->Waiting.clear();
}