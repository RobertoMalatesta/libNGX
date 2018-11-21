#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount)
        : TPoolLock(), TPool(ThreadCount) {}

RuntimeError EventDomain::EventDomainProcess() {
    return {0};
}

RuntimeError EventDomain::PostPromise(PromiseCallback *Callback, void *Argument) {
    LockGuard Guard(&TPoolLock);
    TPool.PostPromise(Callback, Argument);
    return {0};
}
