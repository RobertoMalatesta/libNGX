#include "Core/Core.h"
#include <unistd.h>

using namespace ngx::Core;

Promise::Promise() : Queue() {
}

Promise::Promise(ThreadPool *TPool, Thread *T, PromiseCallback *Callback, void *PointerToArgs)
        : Queue((Queue *) &T->Sentinel, false) {
    this->TPool = TPool;
    this->Callback = Callback;
    this->PointerToArg = PointerToArgs;
}

void Promise::doPromise() {
    if (Callback != nullptr) {
        Callback(PointerToArg, TPool);
    }
}

Thread::Thread(ThreadPool *TPool)
        : Sentinel(), WorkerThread(Thread::ThreadProcess, this), Allocator(THREAD_POOL_MEMORY_SIZE) {
    this->TPool = TPool;
}

Thread::~Thread() {
    Stop();
}

int Thread::TryPostPromise(PromiseCallback *Callback, void *Argument) {

    if (Lock.test_and_set()) {
        return -1;
    }

    if (!Running) {
        return 0;
    }

    void *PointerToPromise = Allocator.Allocate(sizeof(Promise));

    if (nullptr == PointerToPromise) {
        Lock.clear();
        return -1;
    }

    new(PointerToPromise) Promise(TPool, this, Callback, Argument);

    if (PostCount++ % THREAD_GC_ROUND == 0) {
        Allocator.GC();
    }

    Lock.clear();

    return 0;
}

void Thread::ThreadProcess(Thread *Thread) {

    Promise *Node;
    bool IsRunnig = true;

    usleep(THREAD_WAIT_TIME);

    while (IsRunnig) {

        usleep(THREAD_WAIT_TIME);

        while (Thread->Lock.test_and_set()) {
            std::this_thread::yield();
        }

        while (!Thread->Sentinel.IsEmpty()) {

            Node = (Promise *) Thread->Sentinel.GetHead();
            Node->Detach();
            Node->doPromise();
            Thread->Allocator.Free((void **) &Node);
        }

        IsRunnig = Thread->Running;
        Thread->Lock.clear();
    }
}

void Thread::Stop() {

    while (Lock.test_and_set()) {
        RelaxMachine();
    }
    Running = false;
    Lock.clear();
    WorkerThread.join();
    Allocator.GC();
}

ThreadPool::ThreadPool(int NumThread) : NumThread(NumThread) {

    while (NumThread-- > 0) {
        Threads.push_back(new Thread(this));
    }
}

ThreadPool::~ThreadPool() {

    for (Thread *Temp: Threads) {
        delete Temp;
    }
    Threads.clear();
}

void ThreadPool::PostPromise(PromiseCallback *Callback, void *PointerToArg) {

    int RetCode, i = DeliverIndex;

    do {
        RetCode = Threads[(i) % NumThread]->TryPostPromise(Callback, PointerToArg);

        if (RetCode == 0) {
            break;
        }

        if (((i -= RetCode) - DeliverIndex) % NumThread == 0) {
            usleep(THREAD_POOL_SPIN_TIME);
        }
    } while (true);

    DeliverIndex = i;
}
