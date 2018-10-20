#include "Core/Core.h"

using namespace ngx::Core;

Server::Server() : ListeningSentinel(), ConnectionSentinel(){
    Lock.Unlock();
}

EventError Server::EnqueueListening(Listening *L) {

    Queue *PQueue;
    Listening *PListen;

    for (PQueue = ListeningSentinel.GetHead(); PQueue != ListeningSentinel.GetSentinel(); PQueue = PQueue->GetNext()) {
        PListen = (Listening *)(PQueue);

        if (PListen == L) {
            return EventError(EALREADY, "Listen is already added to the Queue");
        }
    }

    Lock.Lock();
    ListeningSentinel.Append(L);
    Lock.Unlock();

    return EventError(0);
}

Listening *Server::DequeueListening() {
    Listening *Listen;
    Lock.Lock();

    if (ListeningSentinel.IsEmpty()) {
        Lock.Unlock();
        return nullptr;
    }

    Listen = (Listening *)ListeningSentinel.GetHead();
    Listen->Detach();

    Lock.Unlock();
    return Listen;
}

EventError Server::AttachConnection(Connection *C) {

    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }

    if (MaxConnection.fetch_sub(1) < 0) {
        MaxConnection.fetch_add(1);
        return EventError(ECANCELED, "Connection reaches maximum connection count!");
    }

    Lock.Lock();
    C->AttachSocket(&ConnectionSentinel);
    Lock.Unlock();
    return EventError(0);
}

EventError Server::DetachConnection(Connection *C) {
    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }

    Lock.Lock();
    C->DetachSocket();
    Lock.Unlock();
    MaxConnection.fetch_add(1);

    return EventError(0);
}

RuntimeError Server::PostProcessFinished(EventPromiseArgs *) {
    return RuntimeError(EINVAL);
}

RuntimeError Server::PostConnectionEvent(EventPromiseArgs *) {
    return RuntimeError(EINVAL);
}
