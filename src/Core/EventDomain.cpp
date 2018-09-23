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

RuntimeError EventDomain::EventDomainProcess(SocketEventDomainArgument* PointerToArgument) {

    if (nullptr == Timers) {
        return RuntimeError(EINVAL, "Timer init failed, Timer == nullptr!");
    }

    Timers->QueueExpiredTimer(&TPool);

    return RuntimeError(0);
}

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) : EventDomain(PoolSize, ThreadCount) {
    this->EventPromise = &DiscardPromise;
}

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount, PromiseCallback *EventPromise) : EventDomain(PoolSize, ThreadCount) {
    this->EventPromise = EventPromise;
}