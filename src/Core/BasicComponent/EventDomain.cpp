#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(size_t PoolSize, int ThreadCount)
        : Lock(), Allocator(PoolSize), TPool(ThreadCount) {}

RuntimeError EventDomain::EventDomainProcess() {
    return {0};
}

RuntimeError EventDomain::PostPromise(PromiseCallback *Callback, void *Argument) {
    TPool.PostPromise(Callback, Argument);
    return {0};
}
