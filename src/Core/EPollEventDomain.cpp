#include "Core/Core.h"

using namespace ngx::Core;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize, PromiseCallback *ProcessPromise) :
        SocketEventDomain(PoolSize, ThreadCount) {
    EPollFD = epoll_create(EPollSize);
    this->EventPromise = ProcessPromise;
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

    if (Event.events | (EPOLLIN | EPOLLRDHUP)) {
        SetSocketReadAttached(S, 1);
    }

    if (Event.events | (EPOLLOUT)) {
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
        Lock.Unlock();
        return EventError(errno, "Failed to attach connection to epoll!");
    }

    SetSocketReadAttached(S, (Event.events & (EPOLLIN | EPOLLRDHUP)) == 0 ? 0 : 1 );
    SetSocketWriteAttached(S, (Event.events & (EPOLLOUT)) == 0 ? 0 : 1 );

    Lock.Unlock();

    return EventError(0);
}

RuntimeError EPollEventDomain::EventDomainProcess(EventPromiseArgs *Arguments) {

    sigset_t sigmask;

    int EventCount = -1;
    epoll_event *Events;

    void *TempPointer;
    int TempConnectionFD;
    SocketAddress *TempSocketAddr;
    socklen_t TempSocketLength;
    Socket *TempSocket;
    EventPromiseArgs *TempEventArguments;
    Listening *Listen;
    Connection *C;
    Server *Server;

    /* [TODO] should move to initialize code latter! */

    RuntimeError Error = EventDomain::EventDomainProcess(Arguments);

    if (Error.GetErrorCode() != 0) {
        return Error;
    }

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);

    if (-1 == EPollFD) {
        return RuntimeError(ENOENT, "EPollEventDomain initial failed!");
    }

    Listen = static_cast<Listening *>(Arguments->UserArguments[5].Ptr);

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

    Arguments->UserArguments[0].Ptr = (void *)Events;

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
        Server = static_cast<class Server *>(Arguments->UserArguments[3].Ptr);

        for (int i=0; i<EventCount; i++) {

            if (Events[i].data.ptr == nullptr) {
                continue;
            }

            TempSocket = static_cast<Socket *>(Events[i].data.ptr);

            TempPointer = Allocate((sizeof(EventPromiseArgs)));

            if (TempPointer == nullptr) {
                continue;
            }

            memcpy(TempPointer, Arguments, sizeof(EventPromiseArgs));

            TempEventArguments = static_cast<EventPromiseArgs *>(TempPointer);
            TempEventArguments->UserArguments[7].UInt = 0;

            if (TempSocket->GetSocketFD() == Listen->GetSocketFD()) {

                TempPointer = Allocate(sizeof(SocketAddress));

                if (TempPointer == nullptr) {
                    continue;
                }

                TempSocketAddr = static_cast<SocketAddress *>(TempPointer);
                TempConnectionFD = accept4(Listen->GetSocketFD(), &TempSocketAddr->sockaddr, &TempSocketLength, SOCK_NONBLOCK);

                if ( -1 == TempConnectionFD) {
                    printf("accept4() failed in HttpEventProcessPromise!\n");
                }

                TempEventArguments->UserArguments[0].UInt = (uint32_t)TempConnectionFD;
                TempEventArguments->UserArguments[1].Ptr = (void *)TempSocketAddr;
                TempEventArguments->UserArguments[2].UInt = TempSocketLength;

                Server->PostNewConnection(TempEventArguments, &TPool);
            }
            else {
                C = (Connection *)TempSocket;
                TempEventArguments->UserArguments[6].Ptr = C;
                if (Events[i].events & (EPOLLIN | EPOLLRDHUP)) {
                    TempEventArguments->UserArguments[7].UInt |= ET_READ;
                    Server->PostConnectionRead(TempEventArguments, &TPool);
                }
                if (Events[i].events & (EPOLLOUT)){
                    TempEventArguments->UserArguments[7].UInt |= ET_WRITE;
                    Server->PostConnectionWrite(TempEventArguments, &TPool);
                }
            }
        }

        TPool.PostPromise(EventPromise, (void *)Arguments);
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

