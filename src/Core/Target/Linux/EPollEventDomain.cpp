#include "Core/Target/Linux/EPollEventDomain.h"

#include <cerrno>
#include <signal.h>
#include <unistd.h>
#include <sys/epoll.h>

using namespace std;
using namespace ngx::Core::Target::Linux;
using namespace ngx::Core::Target::Linux::Config;

static bool SignalSetInited = false;
static sigset_t epoll_sig_mask = {0};

EPollEventDomain::EPollEventDomain(int numThreads, int ePollSize) : EventDomain(numThreads) {

    if (!SignalSetInited) {
        sigemptyset(&epoll_sig_mask);
        sigaddset(&epoll_sig_mask, SIGALRM);
        SignalSetInited = true;
    }
    EPollFD = epoll_create(ePollSize);
}

EPollEventDomain::~EPollEventDomain() {
    if (EPollFD != -1) {
        ::close(EPollFD), EPollFD = -1;
    }
}

EventError EPollEventDomain::attachFD(int FD, uint32_t E, void *pData) {

    epoll_event Event={0};
    Event.events = E;
    Event.data.ptr = pData;

    if (EPollFD == -1) {
        return {EBADF, "EPollEventDomain attachFD() failed, bad EPollFD"};
    } else if (FD == -1) {
        return {EBADF, "EPollEventDomain attachFD() failed, bad FD"};
    } else if (-1 == epoll_ctl(EPollFD, EPOLL_CTL_ADD, FD, &Event)) {
        return {EBADF, "EPollEventDomain attachFD() failed in epoll_ctl()"};
    }
    return {0};
}

EventError EPollEventDomain::detachFD(int FD) {

    epoll_event Event={0};
    Event.events = 0;
    Event.data.ptr = nullptr;

    if (EPollFD == -1) {
        return {EBADF, "EPollEventDomain attachFD() failed, bad EPollFD"};
    } else if (FD == -1) {
        return {EBADF, "EPollEventDomain attachFD() failed, bad FD"};
    } else if (-1 == epoll_ctl(EPollFD, EPOLL_CTL_DEL, FD, &Event)) {
        return {EBADF, "EPollEventDomain attachFD() failed in epoll_ctl()"};
    }
    return {0};
}

EventError EPollEventDomain::attachListen(Listen &L) {
    return attachFD(L.getFD(), EPOLLIN|EPOLLOUT|EPOLLRDHUP, static_cast<EventSubscriber *>(&L));
}
EventError EPollEventDomain::detachListen(Listen &L) {
    return detachFD(L.getFD());
}

EventError EPollEventDomain::attachConnection(Connection &C) {
    return attachFD(C.getFD(), EPOLLET|EPOLLIN|EPOLLOUT|EPOLLRDHUP, static_cast<EventSubscriber *>(&C));
}
EventError EPollEventDomain::detachConnection(Connection &C) {
    return detachFD(C.getFD());
}

RuntimeError EPollEventDomain::eventLoop() {

    int Count;
    RuntimeError Error{0};
    epoll_event Events[EPOLL_EVENT_BATCH_SIZE];

    {
        std::lock_guard<spin_lock> g(Lock);

        if (EPollFD==-1)
            return {ENOENT, "EPollEventDomain, bad epoll fd"};

        Count = epoll_pwait(EPollFD, Events, EPOLL_EVENT_BATCH_SIZE, EPOLL_EVENT_WAIT_TIME, &epoll_sig_mask);
    }

    if (-1 == Count && errno == EINTR)
        return {0, "EPollEventDomain, interrupted by signal"};
    else if (Count <= -1)
        return {errno, "EPollEventDomain epoll_pwait() failed!"};
    else if (Count > 0) {
        for (int i = 0; i < Count; i++) {
            if (Events[i].data.ptr == nullptr)
                continue;

            Event_t E=0;
            auto *Sub= static_cast<EventSubscriber *>(Events[i].data.ptr);

            if (Events[i].events & EPOLLIN)
                E |= Event::READ;
            if (Events[i].events & EPOLLRDHUP)
                E |= Event::CLOSED;
            if (Events[i].events & EPOLLOUT)
                E |= Event::WRITE;

            Sub->postEvent(E);
        }
    }

    return (Error=EventDomain::eventLoop()).GetCode()!=0? Error:0;
}

