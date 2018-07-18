#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise SleepyPromise;

    Promise *Sleep(Promise *, void * ) {
        usleep(20);
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

    ThreadPool::ThreadPool(MemAllocator *Allocator, int NumThread){
        Threads.clear();
        this->Allocator = Allocator;
        this->NumThread = NumThread;
        this->Sentinel.Allocator = Allocator;
    }

    void ThreadPool::ThreadProcess(ThreadPool *Pool) {

        Promise *Node;

        do {
            Pool->Lock();
            usleep(10);

            if (Pool -> Sentinel.IsEmpty()) {
                Node = &SleepyPromise;
            }
            else {
                Node = (Promise *)Pool -> Sentinel. GetLast();
                Node->Detach();
            }

            Pool->Unlock();

            Node->doPromise();

        }while(Pool-> Running.test_and_set());
    }

    void ThreadPool::Start() {

        if (!Threads.empty()) {
            return;
        }

        Running.test_and_set();

        for (int i=0; i < NumThread; i++) {
            Threads.push_back(new thread(ThreadProcess, this));
        }
    }

    void ThreadPool::Stop() {

        while(Running.test_and_set()) {
            Running.clear();
            usleep(1000);
        }

        for(thread *t: Threads) {
            t -> join();
        }
        Threads.clear();
    }
}