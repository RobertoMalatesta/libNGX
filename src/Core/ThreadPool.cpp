#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise *Sleep(Promise *, void * ) {
        usleep(20);
        return nullptr;
    }

    Promise::Promise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArgs):
            Queue(Pool->Sentinel, false) {

        this->Callback = Callback;
        this->PointerToArg = PointerToArgs;
        this->Allocator = Pool -> Allocator;
    }

    void Promise::CleanSelf() {
        if (this->Allocator != nullptr) {
            Promise *Self = this;
            this->Allocator->Free((void **)&Self);
        }
    }

    Promise *Promise::doPromise() {

        Promise *Next = nullptr;

        if (Callback != nullptr) {
            Next = Callback(this, PointerToArg);
        }
        CleanSelf();

        return Next;
    }

    Promise* Promise::CreatePromise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArg) {

        MemAllocator *PoolMemAllocator = Pool -> Allocator;

        void *PointerToMemory = PoolMemAllocator->Allocate(sizeof(Promise));

        if (nullptr == PointerToMemory) {
            return nullptr;
        }

        return new(PointerToMemory) Promise(Pool, Callback, PointerToArg);
    }

    void ThreadPool::PostPromise(Promise *P) {
    }
}