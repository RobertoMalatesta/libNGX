#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    static const Promise SleepPromise;

    static Promise *Sleep(Promise *, void * ) {
        usleep(20);
        return nullptr;
    }

    Promise::Promise() {
        Callback = Sleep;
        PointerToArg = nullptr;
        Allocator = nullptr;
        TPool = nullptr;
    }

    void Promise::CleanSelf() {
        if (this->Allocator != nullptr) {
            Promise *Self = this;
            this->Allocator->Free((void **)&Self);
        }
    }

    void Promise::doPromise() {

        Promise *Next;

        if (Callback != nullptr) {
            Next = Callback(this, PointerToArg);

            if (nullptr != Next) {
                this->TPool->PostPromise(Next);
            }
        }
        CleanSelf();
    }

    Promise* Promise::CreatePromise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArg, MemAllocator *Allocator) {

        void *PointerToMemory = Allocator->Allocate(sizeof(Promise));

        if (nullptr == PointerToMemory) {
            return nullptr;
        }

        Promise * Ret= new(PointerToMemory) Promise();

        Ret->TPool = Pool;
        Ret->Allocator = Allocator;
        Ret->Callback = Callback;
        Ret->PointerToArg = PointerToArg;

        return Ret;

    }

    const Promise *Promise::GetIdlePromise() {
        return &SleepPromise;
    }

    void ThreadPool::PostPromise(Promise *P) {

    }
}