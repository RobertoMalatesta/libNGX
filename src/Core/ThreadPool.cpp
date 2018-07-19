#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise SleepyPromise;

    Promise *Sleep(Promise *, void * ) {
        usleep(500);
        return nullptr;
    }

    Promise::Promise(ThreadPool *Pool): Queue() {
        if (nullptr != Pool) {
            this->Allocator = Pool -> Allocator;
        }
    }

    Promise::Promise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArgs)
            : Queue((Queue *)&Pool->Sentinel, false) {

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

        Pool->Lock();

        if (nullptr == PointerToMemory) {
            return -1;
        }

        new(PointerToMemory) Promise(Pool, Callback, PointerToArg);

        Pool->Unlock();

        return 0;
    }

    ThreadPool::ThreadPool(MemAllocator *Allocator, int NumThread){
        Threads.clear();
        this->Allocator = Allocator;
        this->NumThread = NumThread;
        this->Sentinel.Allocator = Allocator;
    }

    void ThreadPool::ThreadProcess(ThreadPool *Pool) {

        Promise *Head, *Node, *Next;    // [TODO] 添加一次拉取多条Promise

        bool IsRunning;

        do {
            Pool->Lock();
            usleep(10);

            if (Pool -> Sentinel.IsEmpty()) {
                Head = &SleepyPromise;
            }
            else {
                Head = (Promise *) Pool->Sentinel.GetHead();
                Head ->Detach();
            }

            IsRunning = Pool -> Running;
            Pool->Unlock();

            Head->doPromise();

        }while(IsRunning);
    }

    void ThreadPool::Start() {

        if (!Threads.empty()) {
            return;
        }

        Running = true;

        for (int i=0; i < NumThread; i++) {
            Threads.push_back(new thread(ThreadProcess, this));
        }
    }

    void ThreadPool::Stop() {

        Lock();
        Running = false;
        Unlock();

        for(thread *t: Threads) {
            t -> join();
            delete t;
        }

        Threads.clear();
    }
}