#include "Core/Core.h"

using namespace ngx::Core;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount) : Allocator(PoolSize){
    Timers = TimerTree::CreateFromAllocator(&Allocator, &Allocator);
    TPool = new ThreadPool(ThreadCount);
}

EventDomain::~EventDomain() {
    delete TPool;
    TimerTree::FreeFromAllocator(&Allocator, &Timers);
}

RuntimeError EventDomain::PostTimerEvent(uint32_t Seconds, PromiseCallback *Callback, void *Argument) {

    if (nullptr == Timers) {
        return RuntimeError(EINVAL, "Timer init failed, Timer == nullptr!");
    }

    Timers->PostTimerPromise(Seconds, Callback, Argument);

    return RuntimeError(0);
}

RuntimeError EventDomain::EventDomainProcess() {

    if (nullptr == Timers || nullptr == TPool) {
        return RuntimeError(EINVAL, "Timer init failed, Timer == nullptr!");
    }

    Timers->QueueExpiredTimer(TPool);

    return RuntimeError(0);
}
