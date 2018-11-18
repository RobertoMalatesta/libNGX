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

EventError EPollEventDomain::AttachSocket(Socket *S, SocketEventType Type) {

    bool ReadAttached = IsSocketReadAttached(S),
            WriteAttached = IsSocketWriteAttached(S),
            Detached = !(ReadAttached || WriteAttached);
    int SocketFD = S->GetSocketFD();

//    printf("Attach Socket:%d, Type: %x\n", SocketFD, Type);

    if ((Type == SOCK_READ_EVENT && ReadAttached) ||
        (Type == SOCK_WRITE_EVENT && WriteAttached) ||
        (Type == SOCK_READ_WRITE_EVENT && ReadAttached && WriteAttached)
            ) {
        return {EALREADY, "Socket already attached!"};
    }

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    unsigned int EPollCommand = EPOLL_CTL_ADD;
    struct epoll_event Event = {0};
    Event.data.ptr = static_cast<void *> (S);
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
        default:
            break;
    }

    {
        SpinlockGuard LockGuard(&Lock);

        if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
            return {errno, "Failed to attach connection to epoll!"};
        }
    }

    if (Event.events | (EPOLLIN | EPOLLRDHUP)) {
        SetSocketReadAttached(S, 1);
    }

    if (Event.events | (EPOLLOUT)) {
        SetSocketWriteAttached(S, 1);
    }

    return {0};
}

EventError EPollEventDomain::DetachSocket(Socket *S, SocketEventType Type) {

    bool ReadAttached = IsSocketReadAttached(S),
            WriteAttached = IsSocketWriteAttached(S),
            Attached = ReadAttached || WriteAttached;
    int SocketFD = S->GetSocketFD();

//    printf("Detach Socket:%d, Type: %x\n", SocketFD, Type);

    if ((Type == SOCK_READ_EVENT && !ReadAttached) ||
        (Type == SOCK_WRITE_EVENT && !WriteAttached) ||
        (Type == SOCK_READ_WRITE_EVENT && !Attached)) {
        return {EALREADY, "Socket already detached!"};
    }

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    unsigned int EPollCommand = EPOLL_CTL_DEL;

    struct epoll_event Event = {
            .data.ptr = static_cast<void *> (S),
            .events = ((ReadAttached) ? EPOLLIN | EPOLLRDHUP : 0) | (WriteAttached ? EPOLLOUT : 0)
    };

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
        default:
            break;
    }

    if (Event.events == 0) {
        EPollCommand = EPOLL_CTL_DEL;
    }

    {
        SpinlockGuard LockGuard(&Lock);

        if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
            return {errno, "Failed to attach connection to epoll!"};
        }
    }

    SetSocketReadAttached(S, (Event.events & (EPOLLIN | EPOLLRDHUP)) == 0 ? 0 : 1);
    SetSocketWriteAttached(S, (Event.events & (EPOLLOUT)) == 0 ? 0 : 1);

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
