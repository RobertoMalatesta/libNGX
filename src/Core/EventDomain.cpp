#include "Core/Core.h"

using namespace ngx::Core;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount) : Allocator(PoolSize), TPool(ThreadCount){
    Timers = TimerTree::CreateFromAllocator(&Allocator, &Allocator);
    Lock.Unlock();
}

EventDomain::~EventDomain() {
    TimerTree::FreeFromAllocator(&Allocator, &Timers);
}

RuntimeError EventDomain::PostTimerEvent(uint32_t Seconds, PromiseCallback *Callback, void *Argument) {

    if (nullptr == Timers) {
        return RuntimeError(EINVAL, "Timer init failed, Timer == nullptr!");
    }

    Timers->PostTimerPromise(Seconds, Callback, Argument);

    return RuntimeError(0);
}


