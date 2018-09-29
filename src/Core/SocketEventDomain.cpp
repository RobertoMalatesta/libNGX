#include "Core/Core.h"

using namespace ngx::Core;

RuntimeError EventDomain::EventDomainProcess(void* PointerToArgument) {

    if (nullptr == Timers) {
        return RuntimeError(EINVAL, "Timer init failed, Timer == nullptr!");
    }

    Timers->QueueExpiredTimer(&TPool);

    return RuntimeError(0);
}

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) : EventDomain(PoolSize, ThreadCount) {
}
