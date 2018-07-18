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

    int Promise::PostPromise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArg) {

        MemAllocator *PoolMemAllocator = Pool -> Allocator;

        void *PointerToMemory = PoolMemAllocator->Allocate(sizeof(Promise));

        while(Pool->PromiseQueueLock.test_and_set()) {
            RelaxMachine();
        }

        if (nullptr == PointerToMemory) {
            return -1;
        }

        new(PointerToMemory) Promise(Pool, Callback, PointerToArg);

        Pool->PromiseQueueLock.clear();
        return 0;
    }

}