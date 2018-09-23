#include "Core/Core.h"

using namespace ngx::Core;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize, PromiseCallback *ProcessPromise) :
        SocketEventDomain(PoolSize, ThreadCount, ProcessPromise) {
    EPollFD = epoll_create(EPollSize);
    Lock.Unlock();
}

EPollEventDomain::~EPollEventDomain() {
    if (EPollFD != -1) {
        close(EPollFD);
    }
}

EventError EPollEventDomain::AttachSocket(Socket *S, SocketEventType Type) {

    bool ReadAttached = IsSocketReadAttached(S),
            WriteAttached = IsSocketWriteAttached(S),
            Detached = !(ReadAttached || WriteAttached);
    int SocketFD = S->GetSocketFD();

    if ((Type == SOCK_READ_EVENT && ReadAttached) ||
        (Type == SOCK_WRITE_EVENT && WriteAttached) ||
        (Type == SOCK_READ_WRITE_EVENT && ReadAttached && WriteAttached)
            ) {
        return EventError(EALREADY, "Socket already attached!");
    }

    if (EPollFD == -1 || SocketFD == -1) {
        return EventError(-1, "Bad Socket Descriptor!");
    }

    unsigned int EPollCommand = EPOLL_CTL_ADD;
    struct epoll_event Event;
    Event.data.ptr = (void *) S;
    Event.events = 0;

    if (!Detached) {
        EPollCommand = EPOLL_CTL_MOD;

        if (ReadAttached) {
            Event.events |= EPOLLIN | EPOLLRDHUP;
        }

        if (WriteAttached) {
            Event.events |= EPOLLOUT;
        }
    }

    switch (Type) {
        case SOCK_READ_EVENT:
            Event.events |= EPOLLIN | EPOLLRDHUP;
            break;
        case SOCK_WRITE_EVENT:
            Event.events |= EPOLLOUT;
            break;
        case SOCK_READ_WRITE_EVENT:
            Event.events |= EPOLLIN | EPOLLRDHUP | EPOLLOUT;
            break;
    }

    Lock.Lock();

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        Lock.Unlock();
        return EventError(errno, "Failed to attach connection to epoll!");
    }

    if (EPollCommand | (EPOLLIN | EPOLLRDHUP)) {
        SetSocketReadAttached(S, 1);
    }

    if (EPollCommand | (EPOLLOUT)) {
        SetSocketWriteAttached(S, 1);
    }

    Lock.Unlock();
    return EventError(0);
}

EventError EPollEventDomain::DetachSocket(Socket *S, SocketEventType Type) {

    bool ReadAttached = IsSocketReadAttached(S),
            WriteAttached = IsSocketWriteAttached(S),
            Attached = ReadAttached || WriteAttached;
    int SocketFD = S->GetSocketFD();

    if ((Type == SOCK_READ_EVENT && !ReadAttached) ||
        (Type == SOCK_WRITE_EVENT && !WriteAttached) ||
        (Type == SOCK_READ_WRITE_EVENT && !Attached)
            ) {
        return EventError(EALREADY, "Socket already attached!");
    }

    if (EPollFD == -1 || SocketFD == -1) {
        return EventError(-1, "Bad Socket Descriptor!");
    }

    unsigned int EPollCommand = EPOLL_CTL_DEL;
    struct epoll_event Event;
    Event.data.ptr = (void *) S;
    Event.events = 0;

    if (ReadAttached) {
        Event.events |= EPOLLIN | EPOLLRDHUP;
    }

    if (WriteAttached) {
        Event.events |= EPOLLOUT;
    }

    switch (Type) {
        case SOCK_READ_EVENT:
            Event.events &= ~(EPOLLIN | EPOLLRDHUP);
            break;
        case SOCK_WRITE_EVENT:
            Event.events &= ~(EPOLLOUT);
            break;
        case SOCK_READ_WRITE_EVENT:
            Event.events &= ~(EPOLLIN | EPOLLRDHUP | EPOLLOUT);
            break;
    }

    if (Event.events == 0) {
        EPollCommand = EPOLL_CTL_DEL;
    }

    Lock.Lock();

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        return EventError(errno, "Failed to attach connection to epoll!");
    }

    if (EPollCommand | (EPOLLIN | EPOLLRDHUP)) {
        SetSocketReadAttached(S, 1);
    }

    if (EPollCommand | (EPOLLOUT)) {
        SetSocketWriteAttached(S, 1);
    }

    Lock.Unlock();

    return EventError(0);
}

RuntimeError EPollEventDomain::EventDomainProcess(SocketEventDomainArgument *PointerToArgument) {

    int EventCount = -1;
    sigset_t sigmask;
    Listening *Listen;
    epoll_event *Events;
    EPollEventDomainArgument *Arguments, *AllocArgument;

    /* [TODO] should move to initialize code latter! */

    RuntimeError Error = EventDomain::EventDomainProcess(PointerToArgument);

    if (Error.GetErrorCode() != 0) {
        return Error;
    }

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);

    if (-1 == EPollFD) {
        return RuntimeError(ENOENT, "EPollEventDomain initial failed!");
    }

    Arguments = (EPollEventDomainArgument *)PointerToArgument;
    Listen = Arguments->Listening;

    if (nullptr == Listen) {
        return RuntimeError(ENOENT, "Listen queue empty!");
    }

    if (-1 == Listen->GetSocketFD()) {
        return RuntimeError(EINVAL, "Invalid socket fd!");
    }

    if (Waiting.test_and_set()) {
        return RuntimeError(EALREADY, "EPollEventDomain is already waiting for events!");
    }

    Events = (epoll_event *)Allocate(EPOLL_EVENT_BATCH_SIZE * sizeof(epoll_event));

    if (nullptr == Events) {
        Waiting.clear();
        return RuntimeError(ENOMEM, "Failed to allocate memory for epoll_event!");
    }
    Arguments->Events = Events;

    AttachSocket(Listen, SOCK_READ_WRITE_EVENT);
    EventCount = epoll_pwait(EPollFD, Events, EPOLL_EVENT_BATCH_SIZE, EPOLL_EVENT_WAIT_TIME, &sigmask);
    DetachSocket(Listen, SOCK_READ_WRITE_EVENT);

    Waiting.clear();

    if (EventCount <= 0) {
        Free((void **)&Events);
        if (-1 == EventCount) {
            if (errno == EINTR) {
                return RuntimeError(0, "Interrupted by signal");
            } else {
                return RuntimeError(errno, "epoll_wait() failed!");
            }
        } else if (EventCount == 0) {
            return RuntimeError(0);
        }
    } else {
        Arguments->EventCount = EventCount;
        TPool.PostPromise(EventPromise, Arguments);
    }

    return RuntimeError(0);
}

void *EPollEventDomain::Allocate(size_t Size) {
    void *Pointer;
    Lock.Lock();
    Pointer = Allocator.Allocate(Size);
    Lock.Unlock();
    return Pointer;
}

void EPollEventDomain::Free(void **Pointer) {
    Lock.Lock();
    Allocator.Free(Pointer);
    Lock.Unlock();
}

void EPollEventDomain::GC() {
    Lock.Lock();
    Allocator.GC();
    Lock.Unlock();
}