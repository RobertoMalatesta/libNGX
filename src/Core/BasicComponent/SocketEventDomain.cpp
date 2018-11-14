#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) :
        EventDomain(PoolSize, ThreadCount) {
}

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