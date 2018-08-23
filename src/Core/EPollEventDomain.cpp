#include "Core/Core.h"

#include <sys/epoll.h>

using namespace ngx::Core;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize): EventDomain(PoolSize, ThreadCount){

    EPollFD = epoll_create(EPollSize);

    if (EPollFD == -1) {
        return;
    }
}

int EPollEventDomain::EPollAdd() {

    struct epoll_event event;

    if (EPollFD > 0) {
        return epoll_ctl( EPollFD, EPOLL_CTL_ADD, &event);
    }
    return -1;
}

int EPollEventDomain::EPollModify() {


}