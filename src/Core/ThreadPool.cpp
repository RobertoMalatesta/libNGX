#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise SleepyPromise;

    Promise *Sleep(Promise *, void * ) {
        usleep(1);
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

        return Next;
    }

    ThreadPool::ThreadPool(MemAllocator *Allocator, int NumThread){
        Threads.clear();
        this->Allocator = Allocator;
        this->NumThread = NumThread;
        this->Sentinel.TPool = this;
    }

    int ThreadPool::PostPromise(PromiseCallback *Callback, void *PointerToArg) {

        Lock();

        if(ProcessedCount++ % 10000 == 0) {
            Allocator->GC();
        }

        void *PointerToMemory = Allocator->Allocate(sizeof(Promise));

        if (nullptr == PointerToMemory) {
            return -1;
        }

        new(PointerToMemory) Promise(this, Callback, PointerToArg);
        Unlock();

        return 0;
    }

    void ThreadPool::ThreadProcess(ThreadPool *Pool) {

        Promise *Head, *Node, *Next;    // [TODO] 添加一次拉取多条Promise

        bool IsRunning;

        do {
            Pool->Lock();

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


            if (Head != &SleepyPromise) {
                Pool->Lock();
                Pool->Allocator->Free((void **)&Head);
                Pool->Unlock();
            }


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

        Allocator->GC();
        Threads.clear();
    }

}