#include "Core/Core.h"

using namespace ngx::Core;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize, PromiseCallback *ProcessPromise):
        EventDomain(PoolSize, ThreadCount), ListenSentinel(-1, nullptr, -1), ConnectionSentinel(-1, nullptr, -1){
    EPollFD = epoll_create(EPollSize);
    EPollProcessPromise = ProcessPromise;
}

EPollEventDomain::~EPollEventDomain() {
    if (EPollFD != -1) {
        close(EPollFD);
    }
}

void EPollEventDomain::FreeAllocatedMemory(void **PointerToMemory) {
    ModifyLock.Lock();
    Allocator.Free(PointerToMemory);
    ModifyLock.Unlock();
}

EventError EPollEventDomain::EPollAttachSocket(Socket *S)  {

    struct epoll_event Event;
    int ConnectionFD = S->GetSocketFD();

    Event.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
    Event.data.ptr = (void *)S;

    if (-1 == EPollFD || -1 == ConnectionFD) {
        return EventError(-1, "Epoll initial failed!");
    }

    if (-1 == epoll_ctl( EPollFD, EPOLL_CTL_ADD, ConnectionFD, &Event)) {
        return EventError(errno, "Failed to attach connection to epoll!");
    }

    return EventError(0);
}

EventError EPollEventDomain::EPollDetachSocket(Socket *S)  {
    epoll_event Event;
    int ConnectionFD = S->GetSocketFD();

    Event.events = 0;
    Event.data.ptr = nullptr;

    if (-1 == EPollFD || -1 == ConnectionFD) {
        return EventError(-1, "Epoll initial failed!");
    }

    if (-1 == epoll_ctl( EPollFD, EPOLL_CTL_DEL, ConnectionFD, &Event)) {
        return EventError(errno, "Failed to attach connection to epoll!");
    }

    return EventError(0);
}

EventError EPollEventDomain::EPollEnqueueListening(ngx::Core::Listening *L) {
    Queue *PQueue;
    Listening *PListen;

    for (PQueue = ListenSentinel.GetHead(); PQueue != ListenSentinel.GetSentinel(); PQueue = PQueue->GetNext()) {
        PListen = (Listening *)(PQueue);

        if (PListen == L) {
            return EventError(EALREADY, "Listen is already added to the Queue");
        }
    }

    ModifyLock.Lock();
    ListenSentinel.Append(L);
    ModifyLock.Unlock();

    return EventError(0);
}

Listening *EPollEventDomain::EPollDequeueListening() {

    Listening *Listen;
    ModifyLock.Lock();

    if (ListenSentinel.IsEmpty()) {
        ModifyLock.Unlock();
        return nullptr;
    }

    Listen = (Listening *)ListenSentinel.GetHead();
    Listen->Detach();

    ModifyLock.Unlock();
    return Listen;
}

EventError EPollEventDomain::EPollAddConnection(Connection *C) {

    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }

    if (MaxConnection.fetch_sub(1) < 0) {
        MaxConnection.fetch_add(1);
        return EventError(ECANCELED, "Connection reaches maximum connection count!");
    }

    ModifyLock.Lock();
    ConnectionSentinel.Append(C);
    ModifyLock.Unlock();
    return EventError(0);
}

EventError EPollEventDomain::EPollRemoveConnection(Connection *C) {

    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }

    ModifyLock.Lock();
    C->Detach();
    ModifyLock.Unlock();
    MaxConnection.fetch_add(1);

    return EventError(0);
}

RuntimeError EPollEventDomain::EventDomainProcess() {
   RuntimeError Error = EventDomain::EventDomainProcess();

    int EventCount = -1;
    epoll_event * Events;
    Listening *Listen;
    Socket *S;

    /* [TODO] should move to initialize code latter! */
    sigset_t sigmask;

    if (Error.GetErrorCode() != 0) {
       return  Error;
   }

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);

    if (-1==EPollFD ) {
        return RuntimeError(ENOENT, "EPollEventDomain initial failed!");
    }

    if (ListenSentinel.IsEmpty()) {
        return RuntimeError(0);
    }

    Listen = EPollDequeueListening();

    if (nullptr==Listen) {
        return RuntimeError(ENOENT, "Listen queue empty!");
    }

    if (-1 == Listen->GetSocketFD()) {
        return RuntimeError(EINVAL, "Invalid socket fd!");
    }

    Events = static_cast<epoll_event *>(Allocator.Allocate(EPOLL_EVENT_BATCH_SIZE * sizeof(epoll_event)));

    if (nullptr == Events) {
        return RuntimeError(ENOMEM, "Failed to allocate memory for epoll_event!");
    }

    if (Waiting.test_and_set()) {
        return RuntimeError(EALREADY, "EPollEventDomain is already waiting for events!");
    }

    EPollAttachSocket(Listen);
    EventCount = epoll_pwait(EPollFD, Events, EPOLL_EVENT_BATCH_SIZE, EPOLL_EVENT_WAIT_TIME, &sigmask);
    EPollDetachSocket(Listen);

    for (int i=0; i< EventCount; i++) {
        S = (Socket *) Events[i].data.ptr;
        if (S != nullptr) {
            EPollDetachSocket(S);
        }
    }

    Waiting.clear();

    if (-1 == EventCount) {
        if (errno == EINTR) {
            EPollEnqueueListening(Listen);
            return RuntimeError(0, "Interrupted by signal");
        }
        else {
            return RuntimeError(errno, "epoll_wait() failed!");
        }
    }
    else if (EventCount == 0) {
        EPollEnqueueListening(Listen);
    }
    else if (EventCount > 0) {
        EPollEventProcessArguments *Arguments;
        Arguments = static_cast<EPollEventProcessArguments *>(Allocator.Allocate(sizeof(EPollEventProcessArguments)));
        if (Arguments == nullptr) {
            return RuntimeError(ENOMEM, "Failed to allocate memory for EPollEventArguments!");
        }

        Arguments->EventCount = EventCount;
        Arguments->EventDomain = this;
        Arguments->Events = Events;
        Arguments->Listening = Listen;

        TPool.PostPromise(EPollProcessPromise, Arguments);
    }

    return RuntimeError(0);
}