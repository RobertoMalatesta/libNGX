#include "Core/Core.h"

using namespace ngx::Core;

Server::Server() : ListeningSentinel(nullptr, 0), ConnectionSentinel(nullptr, 0){
    ProcessFinished = & Server::OnProcessFinished;
    NewConnection = & Server::OnNewConnection;
    ConnectionRead = & Server::OnConnectionRead;
    ConnectionWrite = & Server::OnConnectionWrite;
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
    ConnectionSentinel.Append(C);
    Lock.Unlock();
    return EventError(0);
}

EventError Server::DetachConnection(Connection *C) {
    if (C == nullptr || C->GetSocketFD() == -1) {
        return EventError(EINVAL, "Bad connection!");
    }

    Lock.Lock();
    C->Detach();
    Lock.Unlock();
    MaxConnection.fetch_add(1);

    return EventError(0);
}