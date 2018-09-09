#include "Core/Core.h"

#include <sys/epoll.h>

using namespace ngx::Core;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize): EventDomain(PoolSize, ThreadCount){

    EPollFD = epoll_create(EPollSize);

    if (EPollFD == -1) {
        return;
    }
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
            return EventError(-1);
        }
    }
    return EventError(0);
}

EventError EPollEventDomain::EPollDetachConnection(Connection *C)  {
    return EventError(0);
}


RuntimeError EPollEventDomain::EventDomainProcess() {
    return RuntimeError(0);
}