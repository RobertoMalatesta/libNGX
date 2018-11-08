#include "Core/Core.h"

using namespace ngx::Core;

Server::Server() : ListeningSentinel(), ConnectionSentinel() {
    Lock.Unlock();
}

EventError Server::EnqueueListening(Listening *L) {

    Queue *PQueue;
    Listening *PListen;
    SpinlockGuard LockGuard(&Lock);

    for (PQueue = ListeningSentinel.GetHead(); PQueue != ListeningSentinel.GetSentinel(); PQueue = PQueue->GetNext()) {
        PListen = (Listening *) (PQueue);

        if (PListen == L) {
            return {EALREADY, "Listen is already added to the Queue"};
        }
    }

    ListeningSentinel.Append(L);

    return {0};
}

Listening *Server::DequeueListening() {
    Listening *Listen;
    SpinlockGuard LockGuard(&Lock);

    if (ListeningSentinel.IsEmpty()) {
        return nullptr;
    }

    Listen = (Listening *) ListeningSentinel.GetHead();
    Listen->Detach();

    return Listen;
}

EventError Server::AttachConnection(Connection *C) {

    if (C == nullptr || C->GetSocketFD() == -1) {
        return {EINVAL, "Bad connection!"};
    }

    if (MaxConnection.fetch_sub(1) <= 0) {
        MaxConnection.fetch_add(1);
        return {ECANCELED, "Connection reaches maximum connection count!"};
    }

    SpinlockGuard LockGuard(&Lock);
    C->AttachSocket(&ConnectionSentinel);
    return {0};
}

EventError Server::DetachConnection(Connection *C) {
    if (C == nullptr || C->GetSocketFD() == -1) {
        return {EINVAL, "Bad connection!"};
    }

    SpinlockGuard LockGuard(&Lock);
    C->DetachSocket();
    MaxConnection.fetch_add(1);

    return {EINVAL};
}

RuntimeError Server::PostProcessFinished(EventPromiseArgs &) {
    return {EINVAL};
}

RuntimeError Server::PostConnectionEvent(EventPromiseArgs &) {
    return {EINVAL};
}
