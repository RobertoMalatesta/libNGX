#include "Core/Core.h"
#include <sys/epoll.h>

using namespace ngx::Core;

static bool SignalSetInited = false;
static sigset_t epoll_sig_mask = {0};

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize) :
        SocketEventDomain(PoolSize, ThreadCount) {

    if (!SignalSetInited) {
        sigemptyset(&epoll_sig_mask);
        sigaddset(&epoll_sig_mask, SIGALRM);
        SignalSetInited = true;
    }

    EPollFD = epoll_create(EPollSize);
}

EPollEventDomain::~EPollEventDomain() {
    if (EPollFD != -1) {
        close(EPollFD);
    }
}

EventError EPollEventDomain::AttachSocket(Socket &S, EventType Type) {

    SpinlockGuard LockGuard(&Lock);

    int SocketFD = S.GetSocketFD();

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    struct epoll_event Event = {0};
    uint32_t Attached = GetAttachedEvent(S);

    unsigned int EPollCommand = (Attached == 0? EPOLL_CTL_ADD: EPOLL_CTL_MOD);
    Event.data.ptr = static_cast<void *> (&S);
    Event.events = EPOLLET;

    if (Type & ET_READ) {
        Event.events |= EPOLLIN | EPOLLRDHUP;
    }

    if (Type & ET_READ) {
        Event.events |= EPOLLOUT;
    }

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
            return {errno, "Failed to attach connection to epoll!"};
    }

    SetAttachedEvent(S, Type, true);
    return {0};
}

EventError EPollEventDomain::DetachSocket(Socket &S, EventType Type) {

    SpinlockGuard LockGuard(&Lock);

    int SocketFD = S.GetSocketFD();
    uint32_t Attached = GetAttachedEvent(S);

    Attached &= ~Type;

    unsigned int EPollCommand = (Attached == 0? EPOLL_CTL_DEL: EPOLL_CTL_MOD);

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    struct epoll_event Event = {
            .data.ptr = static_cast<void *> (&S),
            .events = EPOLLET,
    };

    if (Type & ET_READ) {
        Event.events |= EPOLLIN | EPOLLRDHUP;
    }

    if (Type & ET_READ) {
        Event.events |= EPOLLOUT;
    }

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        return {errno, "Failed to attach connection to epoll!"};
    }

    SetAttachedEvent(S, Type, false);
    return {0};
}

RuntimeError EPollEventDomain::EventDomainProcess(Server *S) {

    int EventCount;
    epoll_event Events[EPOLL_EVENT_BATCH_SIZE];

    RuntimeError Error{0}, PostError{0};

    Error = EventDomain::EventDomainProcess();

    if (Error.GetCode() != 0) {
        return Error;
    }

    if (-1 == EPollFD) {
        return {ENOENT, "EPollEventDomain initial failed!"};
    }

    if (Waiting.test_and_set()) {
        return {EALREADY, "EPollEventDomain is already waiting for events!"};
    }

    EventCount = epoll_pwait(EPollFD, Events, EPOLL_EVENT_BATCH_SIZE, EPOLL_EVENT_WAIT_TIME, &epoll_sig_mask);

    Waiting.clear();

    if (-1 == EventCount && errno == EINTR) {
        Error = {0, "Interrupted by signal"};
    } else if (EventCount <= -1) {
        return {errno, "epoll_wait() failed!"};
    } else if (EventCount > 0) {
        for (int i = 0; i < EventCount; i++) {

            if (Events[i].data.ptr == nullptr) {
                continue;
            }

            auto *TempSocket = static_cast<Socket *>(Events[i].data.ptr);

            uint32_t EventType = 0;

            if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
                EventType |= ET_READ;
            }

            if (Events[i].events & (EPOLLOUT)) {
                EventType |= ET_WRITE;
            }

            TempSocket->HandleEventDomain(EventType);
        }
    }

    PostError = S->PostProcessFinished();

    return PostError.GetCode() != 0 ? PostError : Error;
}
