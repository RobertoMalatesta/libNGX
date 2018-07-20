#include <Core/Core.h>
#include <unistd.h>

namespace ngx::Core {

    Promise SleepyPromise;

    Promise *Sleep(Promise *, void *) {
        usleep(20 * 1000);
        return nullptr;
    }

    Promise::Promise(ThreadPool *Pool) : Queue() {
        this->TPool = Pool;
    }

    Promise::Promise(ThreadPool *Pool, PromiseCallback *Callback, void *PointerToArgs)
            : Queue((Queue *) &Pool->Sentinel, false) {
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

    ThreadPool::ThreadPool(MemAllocator *Allocator, int NumThread) {
        Threads.clear();
        this->Allocator = Allocator;
        this->NumThread = NumThread;
        this->Sentinel.TPool = this;
    }

    int ThreadPool::PostPromise(PromiseCallback *Callback, void *PointerToArg) {

        Lock();

        if (ProcessedCount++ % 5000 == 0) {
            Allocator->GC();
        }

        void *PointerToMemory = Allocator->Allocate(sizeof(Promise));

        if (nullptr == PointerToMemory) {
            Unlock();
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

            if (Pool->Sentinel.IsEmpty()) {
                Head = &SleepyPromise;
            } else {
                Head = (Promise *) Pool->Sentinel.GetHead();
                Pool->Sentinel.Detach();
            }

            IsRunning = Pool->Running;
            Pool->Unlock();

            if (Head == &SleepyPromise) {
                Head->doPromise();
            } else {

                Node = Head;
                do {
                    Node->doPromise();
                    Node = (Promise *) Node->GetNext();
                } while (Node != Head);

//                 内存释放存在问题
//                Pool->Lock();
//                Node = Head;
//                do {
//                    Next = (Promise *) Node->GetNext();
//                    Pool->Allocator->Free((void **) &Node);
//                    Node = Next;
//                } while (Node != Head);
//                Pool->Unlock();
            }
        } while (IsRunning);
    }

    void ThreadPool::Start() {

        if (!Threads.empty()) {
            return;
        }

        Running = true;

        for (int i = 0; i < NumThread; i++) {
            Threads.push_back(new thread(ThreadProcess, this));
        }
    }

    void ThreadPool::Stop() {

        Lock();
        Running = false;
        Unlock();

        for (thread *t: Threads) {
            t->join();
            delete t;
        }

        Allocator->GC();
        Threads.clear();
    }

}