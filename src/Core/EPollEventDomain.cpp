#include "Core/Core.h"

#include <sys/epoll.h>

using namespace ngx::Core;

typedef struct {
    EPollEventDomain *EventDomain;
    Listening *Listening;
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

    if (EPollFD != -1 && ConnectionFD != -1) {
        if (-1 == epoll_ctl( EPollFD, EPOLL_CTL_ADD, ConnectionFD, &Event)) {
            return EventError(-1, "Failed to attach connection to epoll!");
        }
    }
    return EventError(0);
}

EventError EPollEventDomain::EPollDetachConnection(Connection *C)  {
    return EventError(0);
}

EventError EPollEventDomain::EPollPostListenPromise(Listening *Listening) {

    if (nullptr==TPool || -1==EPollFD ) {
        return EventError(ENOENT, "EPollEventDomain initial failed!");
    }

    if (nullptr==Listening || -1 == Listening->GetSocketFD()) {
        return EventError(EINVAL, "Invalid input socket!");
    }

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


}