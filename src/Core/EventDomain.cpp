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

void EventDomain::PostTimerEvent(uint32_t Seconds, PromiseCallback *Callback, void *Argument) {

    if (Timers) {
        Timers->PostTimerPromise(Seconds, Callback, Argument);
    }
    else {
        // [WARN] 定时器初始化错误
    }
}

void EventDomain::EventDomainProcess() {
    Timers->QueueExpiredTimer(TPool);
}
