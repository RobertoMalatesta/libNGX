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

    printf("Attach Socket:%d, Type: %x\n", SocketFD, Type);

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
    Event.events = EPOLLET;

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

    SpinlockGuard LockGuard(&Lock);

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        return {errno, "Failed to attach connection to epoll!"};
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

    printf("Detach Socket:%d, Type: %x\n", SocketFD, Type);

    if ((Type == SOCK_READ_EVENT && !ReadAttached) ||
        (Type == SOCK_WRITE_EVENT && !WriteAttached) ||
        (Type == SOCK_READ_WRITE_EVENT && !Attached)) {
        return {EALREADY, "Socket already attached!"};
    }

    if (EPollFD == -1 || SocketFD == -1) {
        return {-1, "Bad Socket Descriptor!"};
    }

    unsigned int EPollCommand = EPOLL_CTL_DEL;

    struct epoll_event Event = {
            .data.ptr = static_cast<void *> (S),
            .events = EPOLLET | ((ReadAttached) ? EPOLLIN | EPOLLRDHUP : 0) | (WriteAttached ? EPOLLOUT : 0)
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
    }

    if (Event.events == 0) {
        EPollCommand = EPOLL_CTL_DEL;
    }

    SpinlockGuard LockGuard(&Lock);

    if (-1 == epoll_ctl(EPollFD, EPollCommand, SocketFD, &Event)) {
        return {errno, "Failed to attach connection to epoll!"};
    }

    SetSocketReadAttached(S, (Event.events & (EPOLLIN | EPOLLRDHUP)) == 0 ? 0 : 1);
    SetSocketWriteAttached(S, (Event.events & (EPOLLOUT)) == 0 ? 0 : 1);

    return {0};
}

RuntimeError EPollEventDomain::EventDomainProcess(EventPromiseArgs &Argument) {

    Server *Server;
    Listening *Listen;
    SocketAddress TempAddress = {0};
    int EventCount, TempConnectionFD;
    epoll_event Events[EPOLL_EVENT_BATCH_SIZE];

    /* [TODO] should move to initialize code latter! */

    RuntimeError Error = EventDomain::EventDomainProcess(Argument);

    if (Error.GetErrorCode() != 0) {
        return Error;
    }

    if (-1 == EPollFD) {
        return {ENOENT, "EPollEventDomain initial failed!"};
    }

    Server = static_cast<class Server *>(Argument.UserArguments[3].Ptr);
    Listen = static_cast<Listening *>(Argument.UserArguments[5].Ptr);

    if (nullptr == Listen || nullptr == Server) {
        return {ENOENT, "Listen queue empty!"};
    }

    if (-1 == Listen->GetSocketFD()) {
        return {EINVAL, "Invalid socket fd!"};
    }

    if (Waiting.test_and_set()) {
        return {EALREADY, "EPollEventDomain is already waiting for events!"};
    }

    AttachSocket(Listen, SOCK_READ_WRITE_EVENT);
    EventCount = epoll_pwait(EPollFD, Events, EPOLL_EVENT_BATCH_SIZE, EPOLL_EVENT_WAIT_TIME, &epoll_sig_mask);
    DetachSocket(Listen, SOCK_READ_WRITE_EVENT);

    Waiting.clear();

    if (-1 == EventCount) {
        if (errno == EINTR) {
            return {0, "Interrupted by signal"};
        } else {
            return {errno, "epoll_wait() failed!"};
        }
    } else if (0 == EventCount) {
        return {0};
    } else if (EventCount < -1) {
        return {errno, "epoll_wait() failed!"};
    } else {

        for (int i = 0; i < EventCount; i++) {

            if (Events[i].data.ptr == nullptr) {
                continue;
            }

            auto *TempSocket = static_cast<Socket *>(Events[i].data.ptr);
            Argument.UserArguments[7].UInt = 0;

            if (TempSocket->GetSocketFD() == Listen->GetSocketFD()) {

                TempConnectionFD = accept4(Listen->GetSocketFD(), &TempAddress.sockaddr,
                                           &TempAddress.SocketLength, SOCK_NONBLOCK);

                if (-1 == TempConnectionFD) {
                    printf("accept4() failed in EventDomainProcess()!\n");
                }

                Argument.UserArguments[0].UInt = (uint32_t) TempConnectionFD;
                Argument.UserArguments[1].Ptr = static_cast<void *>(&TempAddress);
                Argument.UserArguments[6].Ptr = nullptr;
                Argument.UserArguments[7].UInt |= ET_CONNECTED;
            } else {
                Argument.UserArguments[6].Ptr = static_cast<void *>(TempSocket);

                if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
                    Argument.UserArguments[7].UInt |= ET_READ;
                }

                if (Events[i].events & (EPOLLOUT)) {
                    Argument.UserArguments[7].UInt |= ET_WRITE;
                }
            }
            Server->PostConnectionEvent(Argument);
        }
    }

    Argument.UserArguments[0].UInt = 0;
    Argument.UserArguments[1].UInt = 0;
    Argument.UserArguments[6].UInt = 0;
    Argument.UserArguments[7].UInt = 0;

    Server->PostProcessFinished(Argument);

    return {0};
}
