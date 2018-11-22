#include "Core/Core.h"
#include <unistd.h>

using namespace ngx::Core::BasicComponent;

Promise::Promise() {
}

void Promise::doPromise() {
    if (Callback != nullptr) {
        Callback(PointerToArg, TPool);
    }
}

Thread::Thread(ThreadPool *TPool) : Sentinel(), Allocator() {

    int ret;

    this->Running = true;
    this->TPool = TPool;
    this->Lock.Lock();

    ret = pthread_create(&WorkerThread, nullptr,Thread::ThreadProcess, (void *)this);

    if (ret != 0) {
        this->Running = false;
    }
}

int Thread::TryPostPromise(PromiseCallback *Callback, void *Argument) {

    Promise *P = nullptr;

    if (!Lock.TryLock()) {
        return -1;
    }

    if (!Running) {
        return 0;
    }

    if (Build(P) != 0 || P == nullptr) {
        Lock.Unlock();
        return -1;
    }

    P->TPool = TPool;
    P->Callback = Callback;
    P->PointerToArg = Argument;
    Sentinel.Append(&P->Q);

    if (PostCount++ % THREAD_GC_ROUND == 0) {
        Allocator.GC();
    }

    Lock.Unlock();
    return 0;
}

void* Thread::ThreadProcess(void *Argument) {

    Queue *Q;
    Promise *P;

    auto *T = static_cast<Thread *>(Argument);

    T->Lock.Unlock();

    while (true) {

        usleep(THREAD_WAIT_TIME);
        LockGuard Guard(&T->Lock);

        if (!T->Running) {
            break;
        } else {
            while (!T->Sentinel.IsEmpty()) {
                Q = T->Sentinel.GetLast();
                P = Promise::FromQueue(Q);

                Q->Detach();
                P->doPromise();

                T->Destroy(P);
            }
        }
    }

    T->Lock.Unlock();
    return nullptr;
}

void Thread::Stop() {
    {
        LockGuard LockGuard(&Lock);
        Running = false;
    }
    pthread_join(WorkerThread, nullptr);
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

