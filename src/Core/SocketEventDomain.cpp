#include "Core/Core.h"

using namespace ngx::Core;

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) :
    EventDomain(PoolSize, ThreadCount) {
}

RuntimeError EventDomain::EventDomainProcess(EventPromiseArgs *PointerToArgument) {

    if (nullptr == Timers) {
        return RuntimeError(EINVAL, "Timer init failed, Timer == nullptr!");
    }

    Timers->QueueExpiredTimer(&TPool);

    return RuntimeError(0);
}

RuntimeError EventDomain::PostPromise(PromiseCallback *Callback, void *Argument) {
    TPool.PostPromise(Callback, Argument);
    return RuntimeError(0);
}