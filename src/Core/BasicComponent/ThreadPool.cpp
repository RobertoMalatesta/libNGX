#include "Core/Core.h"
#include <unistd.h>

using namespace ngx::Core::BasicComponent;

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

RuntimeError Thread::PostPromise(PromiseCallback *Callback, void *Argument) {

    Promise *P = nullptr;

    LockGuard Guard(&Lock);

    if (!Running) {
        return {EFAULT, "thread is not running"};
    }

    if (Build(P) != 0 || P == nullptr) {
        return {ENOMEM, "insufficent memory"};
    }

    P->TPool = TPool;
    P->Callback = Callback;
    P->PointerToArg = Argument;
    Sentinel.Append(&P->Q);

    if (PostCount++ % THREAD_GC_ROUND == 0) {
        Allocator.GC();
    }

    return {0};
}


void* Thread::ThreadProcess(void *Argument) {

    Queue *Q;
    Promise *P;

    auto *T = static_cast<Thread *>(Argument);

    T->Lock.Unlock();

    LOG(INFO) << "thread start, tid: " << pthread_self();

    ForceSleep(NANO_SECOND_SIZE);

    while (true) {

        ForceSleep(THREAD_WAIT_TIME);

        T->Lock.Lock();

        if (!T->Running) {
            T->Lock.Unlock();
            break;
        } else {
            while (!T->Sentinel.IsEmpty()) {
                Q = T->Sentinel.GetNext();
                P = Promise::FromQueue(Q);

                Q->Detach();
                P->doPromise();

                T->Destroy(P);
            }
        }

        T->Lock.Unlock();
    }

    LOG(INFO) << "thread exit, tid: " << pthread_self();

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

Thread *ThreadPool::GetOneThread() {
    return Threads[DeliverIndex++ % NumThread];
}

