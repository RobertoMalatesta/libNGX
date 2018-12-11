#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(int ThreadCount)
        : TPoolLock(), TPool(ThreadCount) {}

RuntimeError EventDomain::EventDomainProcess() {
    return {0};
}

RuntimeError EventDomain::BindEventThread(EventEntity &Entity) {

    Thread *T = TPool.GetOneThread();

    if (T == nullptr) {
        return {ENOENT, "can not get a thread"};
    }

    Entity.BackendWorker = T;

    return {0};
}

RuntimeError EventDomain::UnbindEventThread(EventEntity &Entity) {
    Entity.BackendWorker = nullptr;
    return {0};
}

RuntimeError EventEntity::PostPromise(PromiseCallback *Callback, void *Argument) {

    if (BackendWorker == nullptr) {
        return {EFAULT, "no backend worker"};
    }

    return BackendWorker->PostPromise(Callback, Argument);
}