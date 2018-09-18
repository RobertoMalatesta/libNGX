#include "Core/Core.h"

#include <sys/epoll.h>
#include <signal.h>

using namespace ngx::Core;

typedef struct _EPollEventProcessArguments{
    EPollEventDomain *EventDomain;
    int ListeningSocketFD;
    epoll_event *Events;
    int EventCount;
} EPollEventProcessArguments;

EPollEventDomain::EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize):
        EventDomain(PoolSize, ThreadCount), ListenSentinel(-1), ConnectionSentinel(-1){
    EPollFD = epoll_create(EPollSize);
}

EPollEventDomain::~EPollEventDomain() {
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

RuntimeError EPollEventDomain::EPollListenToNext() {

    int EventCount = -1;
    epoll_event * Events;
    Listening *Listen;

    /* [TODO] should move to initialize code latter! */
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);

    if (-1==EPollFD ) {
        return RuntimeError(ENOENT, "EPollEventDomain initial failed!");
    }

    if (ListenSentinel.IsEmpty()) {
        return RuntimeError(0);
    }

    Listen = (Listening *)ListenSentinel.GetNext();

    if (nullptr==Listen) {
        return RuntimeError(ENOENT, "Bad evnet queue!");
    }

    Listen->Detach();

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
    ListenSentinel.Append(Listen);
    Waiting.clear();

    if (-1 == EventCount) {
        if (errno == EINTR) {
            return RuntimeError(0, "Interrupted by signal");
        }
        else {
            return RuntimeError(errno, "epoll_wait() failed!");
        }
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
        Arguments->ListeningSocketFD = Listen->GetSocketFD();

        TPool.PostPromise(&(EPollEventDomain::EPollEventProcessPromise), Arguments);
    }
    return RuntimeError(0);
}

RuntimeError EPollEventDomain::EventDomainProcess() {
    return RuntimeError(0);
}

void EPollEventDomain::EPollEventProcessPromise(void *Args, ThreadPool *TPool) {

    EPollEventProcessArguments *EPollArguments;

    if (nullptr == Args) {
        return;
    }

    EPollArguments = static_cast<EPollEventProcessArguments *>(Args);
    EPollEventDomain *Domain = EPollArguments->EventDomain;
    epoll_event *Events = EPollArguments->Events;
    int EventCount = EPollArguments->EventCount;
    int ListeningSocketFD = EPollArguments->ListeningSocketFD;

    if (Domain == nullptr ||
        Events == nullptr ||
        EventCount <= 0) {
        return;
    }

    Domain->Allocator.Free((void **)&EPollArguments);

    printf("PointerToEventDomain: %p,EventCount: %d,Events: %p, ListeningSocketFD: %d\n", Domain, EventCount, Events,
           ListeningSocketFD);
}

EventError EPollEventDomain::EPollAddListening(Listening *L) {

    Queue *PQueue;
    Listening *PListen;

    for (PQueue = ListenSentinel.GetHead(); PQueue != ListenSentinel.GetSentinel(); PQueue = PQueue->GetNext()) {
        PListen = (Listening *)(PQueue);

        if (PListen == L) {
            return EventError(EALREADY, "Listen is already added to the Queue");
        }
    }

    ListenSentinel.Append(L);
    return EventError(0);
}

EventError EPollEventDomain::EPollRemoveListening(Listening *L) {

    Queue *PQueue;
    Listening *PListen;

    for (PQueue = ListenSentinel.GetHead(); PQueue != ListenSentinel.GetSentinel(); PQueue = PQueue->GetNext()) {
        PListen = (Listening *)(PQueue);

        if (PListen == L) {
            PListen->Detach();
            return EventError(0);
        }
    }

    return EventError(ENOENT, "Listening not found!");
}

EventError EPollEventDomain::EPollAddConnection(Connection *C) {

    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }

    if (MaxConnection.fetch_sub(1) < 0) {
        MaxConnection.fetch_add(1);
        return EventError(ECANCELED, "Connection reaches maximum connection count!");
    }

    ConnectionSentinel.Append(C);
    return EventError(0);
}

EventError EPollEventDomain::EPollRemoveConnection(Connection *C) {

    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }



    C->Detach();
    MaxConnection.fetch_add(1);

    return EventError(0);
}