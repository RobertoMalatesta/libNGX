#include "Core/Core.h"

using namespace ngx::Core::Support;

EventDomain::EventDomain(int NumThreads)
        : Lock(), Pool(NumThreads), Tree() {
}

RuntimeError EventDomain::EventLoop() {
    return {0};
}

RuntimeError EventDomain::BindEventThread(EventEntity &Entity) {

    Thread *T = Pool.fetchOneThread();

    if (T == nullptr) {
        return {ENOENT, "can not get a thread"};
    }

    Entity.BackendWorker = T;

    return {0};
}

RuntimeError EventDomain::UnbindEventThread(EventEntity &Entity) {
    Entity.BackendWorker = nullptr;
    return {0};
}

RuntimeError EventEntity::PostJob(Job &J) {

    if (BackendWorker == nullptr) {
        return {EFAULT, "no backend worker"};
    }

    return BackendWorker->PostJob(J);
}

void EventDomain::EmitExpiredTimer() {

    Connection *C;
    uint64_t Timestamp=GetHighResolutionTimestamp();

    for(Timer *next,*it=Tree.begin(); it; ) {
        if (it->Timestamp > Timestamp) {
            break;
        }

        next=Tree.next(it);

        C=Connection::timerToConnection(it);
        if (C->Lock.TryLock()) {
            Tree.detach(*it);

            C->PostJob(it->TimerJob);

            if (it->Mode == TM_INTERVAL && it->Interval > 0) {
                it->Timestamp = Timestamp + it->Interval;
                Tree.attachTimer(*it);
            } else {
                it->Mode = TM_CLOSED;
            }
            C->Lock.Unlock();
        }
        it=next;
    }
}