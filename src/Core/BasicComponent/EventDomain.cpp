#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

EventDomain::EventDomain(int ThreadCount)
        : TPoolLock(), TPool(ThreadCount) {}

RuntimeError EventDomain::EventLoop() {
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

RuntimeError EventEntity::PostJob(Job &J) {

    if (BackendWorker == nullptr) {
        return {EFAULT, "no backend worker"};
    }

    return BackendWorker->PostJob(J);
}