#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount)
        : Lock(), Allocator(PoolSize), Timers(&Allocator), TPool(ThreadCount) {}

RuntimeError EventDomain::QueueExpiredTimer() {
    Timers.QueueExpiredTimer(&TPool);
    return {0};
}

RuntimeError EventDomain::EventDomainProcess() {
    return QueueExpiredTimer();
}

RuntimeError EventDomain::PostPromise(PromiseCallback *Callback, void *Argument) {
    TPool.PostPromise(Callback, Argument);
    return {0};
}
