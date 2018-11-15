#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount)
        : Lock(), Allocator(PoolSize), Timers(&Allocator), TPool(ThreadCount) {}

RuntimeError EventDomain::QueueExpiredTimer() {
    SpinlockGuard LockGuard(&Lock);
    Timers.QueueExpiredTimer(&TPool);
    return {0};
}

RuntimeError EventDomain::EventDomainProcess(EventPromiseArgs &Argument) {
    return QueueExpiredTimer();
}

RuntimeError EventDomain::PostPromise(PromiseCallback *Callback, void *Argument) {
    SpinlockGuard LockGuard(&Lock);
    TPool.PostPromise(Callback, Argument);
    return {0};
}

