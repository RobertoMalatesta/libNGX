#include "Core/Core.h"
#include <sys/epoll.h>

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

EventError EPollEventDomain::AttachSocket(Socket &S, EventType Type) {

    std::lock_guard<spin_lock> g(TPoolLock);

    int SocketFD = S.GetSocketFD();

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    struct epoll_event Event = {0};
    uint32_t Attached = GetAttachedEvent(S);

    unsigned int EPollCommand = (Attached == 0 ? EPOLL_CTL_ADD : EPOLL_CTL_MOD);

    Attached |= Type;
    Event.data.ptr = static_cast<void *> (&S);

    if ((Attached & ET_ACCEPT) == 0) {
        Event.events |= EPOLLET;
    }

    if (Attached & ET_READ) {
        Event.events |= EPOLLIN | EPOLLRDHUP;
    }

    if (Attached & ET_WRITE) {
        Event.events |= EPOLLOUT;
    }

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        return {errno, "Failed to attach connection to epoll!"};
    }

    SetAttachedEvent(S, Attached, true);
    return {0};
}

EventError EPollEventDomain::DetachSocket(Socket &S, EventType Type) {

    std::lock_guard<spin_lock> g(EPollLock);

    int SocketFD = S.GetSocketFD();

    uint32_t Attached = GetAttachedEvent(S);
    Attached &= ~Type;

    epoll_event Event{0};

    unsigned int EPollCommand = (Attached == 0 ? EPOLL_CTL_DEL : EPOLL_CTL_MOD);

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    Event.data.ptr = static_cast<void *>(&S);

    if ((Attached & ET_ACCEPT) == 0) {
        Event.events |= EPOLLET;
    }

    if (Attached & ET_READ) {
        Event.events |= EPOLLIN | EPOLLRDHUP;
    }

    if (Attached & ET_WRITE) {
        Event.events |= EPOLLOUT;
    }

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        return {errno, "Failed to attach connection to epoll!"};
    }

    SetAttachedEvent(S, Attached, false);
    return {0};
}

RuntimeError EPollEventDomain::EventLoop() {

    int EventCount;
    epoll_event Events[EPOLL_EVENT_BATCH_SIZE];

    RuntimeError Error{0}, PostError{0};

    Error = SocketEventDomain::EventLoop();

    if (Error.GetCode() != 0) {
        return Error;
    }

    EPollLock.lock();
    if (-1 == EPollFD) {
        EPollLock.Unlock();
        return {ENOENT, "EPollEventDomain initial failed!"};
    }

    EventCount = epoll_pwait(EPollFD, Events, EPOLL_EVENT_BATCH_SIZE, EPOLL_EVENT_WAIT_TIME, &epoll_sig_mask);

    EPollLock.Unlock();

    if (-1 == EventCount && errno == EINTR) {
        return {0, "Interrupted by signal"};
    } else if (EventCount <= -1) {
        return {errno, "epoll_wait() failed!"};
    } else if (EventCount > 0) {

        for (int i = 0; i < EventCount; i++) {

            if (Events[i].data.ptr == nullptr) {
                continue;
            }

            auto *TempSocket = static_cast<Socket *>(Events[i].data.ptr);

            uint32_t EventType = 0;

            if (Events[i].events & EPOLLIN || Events[i].events & EPOLLRDHUP) {
                EventType |= ET_READ;
            }

            if (Events[i].events & (EPOLLOUT)) {
                EventType |= ET_WRITE;
            }
            TempSocket->HandleDomainEvent(EventType);
        }
    }

    return {0};
}

RuntimeError EPollEventDomain::attachFD(int FD, uint32_t Event, void *pData) {

    epoll_event Event = {
            .data = {
                    .ptr = pData;
            },
            .events = Event,
    };

    if (EPollFD == -1) {
        return {EBADF, "EPollEventDomain attachFD() failed, bad EPollFD"};
    } else if (FD == -1) {
        return {EBADF, "EPollEventDomain attachFD() failed, bad FD"};
    } else if (-1 == epoll_ctl(EPollFD, EPOLL_CTL_ADD, FD, &Event)) {
        return {EBADF, "EPollEventDomain attachFD() failed in epoll_ctl()"};
    }
    return {0};
}

