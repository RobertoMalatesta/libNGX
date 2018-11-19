#include "Core/Core.h"
#include <unistd.h>

using namespace ngx::Core::BasicComponent;

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

Thread::Thread(ThreadPool *TPool) : Sentinel(), WorkerThread(Thread::ThreadProcess, this), Allocator() {
    this->TPool = TPool;
}

int Thread::TryPostPromise(PromiseCallback *Callback, void *Argument) {


    if (!Lock.TryLock()) {
        return -1;
    }

    if (!Running) {
        return 0;
    }

    void *PointerToPromise = Allocator.Allocate(sizeof(Promise));

    if (nullptr == PointerToPromise) {
        Lock.Unlock();
        return -1;
    }

    new(PointerToPromise) Promise(TPool, this, Callback, Argument);

    if (PostCount++ % THREAD_GC_ROUND == 0) {
        Allocator.GC();
    }

    Lock.Unlock();

    return 0;
}

void Thread::ThreadProcess(Thread *Thread) {

    Promise *Node;
    Thread->Running = true;

    {
        uint64_t Timestamp = GetHighResolutionTimestamp();

        while (GetHighResolutionTimestamp() - Timestamp < (THREAD_WAIT_TIME * 10 + 100000)) {
            usleep(THREAD_WAIT_TIME * 10 + 100000);
        }
    }

    while (true) {

        usleep(THREAD_WAIT_TIME);

        if (Thread->Lock.TryLock()) {
            if (!Thread->Running) {
                break;
            } else {
                while (!Thread->Sentinel.IsEmpty()) {
                    Node = (Promise *) Thread->Sentinel.GetHead();
                    Node->Detach();
                    Node->doPromise();
                    Thread->Allocator.Free((void *&) Node);
                }
                Thread->Lock.Unlock();
            }
        }
    }
    Thread->Lock.Unlock();
}

void Thread::Stop() {
    {
        SpinlockGuard LockGuard(&Lock);
        Running = false;
    }
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
        Temp->Stop(), delete Temp;
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

