#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise SleepyPromise;

    Promise *Sleep(Promise *, void * ) {
        usleep(500);
        return nullptr;
    }

    Promise::Promise(ThreadPool *Pool): Queue() {
        this->TPool = Pool;
    }

    Promise::Promise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArgs)
            : Queue((Queue *)&Pool->Sentinel, false) {
        this->TPool = Pool;
        this->Callback = Callback;
        this->PointerToArg = PointerToArgs;
    }

    Promise *Promise::doPromise() {

        Promise *Next = nullptr;

        if (Callback != nullptr) {
            Next = Callback(this, PointerToArg);
        }

        if (nullptr != TPool) {
            TPool->CleanPromise(this);
        }

        return Next;
    }

    int ThreadPool::PostPromise(PromiseCallback *Callback, void *PointerToArg) {

        Lock();

        void *PointerToMemory = Allocator->Allocate(sizeof(Promise));

        if (nullptr == PointerToMemory) {
            return -1;
        }

        new(PointerToMemory) Promise(this, Callback, PointerToArg);
        Unlock();

        return 0;
    }

    void ThreadPool::CleanPromise(Promise *Promise) {
        Lock();
        Allocator->Free((void **)&Promise);
        Unlock();
    }

    ThreadPool::ThreadPool(MemAllocator *Allocator, int NumThread){
        Threads.clear();
        this->Allocator = Allocator;
        this->NumThread = NumThread;
        this->Sentinel.TPool = this;
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