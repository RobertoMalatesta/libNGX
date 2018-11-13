#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) :
        EventDomain(PoolSize, ThreadCount) {
}

RuntimeError EventDomain::EventDomainProcess(EventPromiseArgs &Argument) {
    Timers.QueueExpiredTimer(&TPool);
    return {0};
}

RuntimeError EventDomain::PostPromise(PromiseCallback *Callback, void *Argument) {
    TPool.PostPromise(Callback, Argument);
    return {0};
}