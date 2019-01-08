#include "Core/Core.h"
#include <unistd.h>

using namespace ngx::Core::BasicComponent;

Job::Job(ThreadFn *Fn, void *Argument) : Callback(Fn), PointerToArg(Argument) {
}

Job::Job(Job &J) : Callback(J.Callback), PointerToArg(J.PointerToArg) {
}

void Job::doJob() {

    if (Callback != nullptr) {
        Callback(PointerToArg);
    }
}

Thread::Thread() : Sentinel(), Allocator() {

    int ret;

    this->Running = true;
    this->Lock.Lock();

    ret = pthread_create(&WorkerThread, nullptr, Thread::ThreadProcess, (void *) this);

    if (ret != 0) {
        this->Running = false;
    }
}

RuntimeError Thread::PostJob(Job &J) {

    Job *J1 = nullptr;

    LockGuard Guard(&Lock);

    if (!Running) {
        return {EFAULT, "thread is not running"};
    }

    if (Build(J1) != 0 || J1 == nullptr) {
        return {ENOMEM, "insufficent memory"};
    }

    *J1 = J;

    Sentinel.Append(&J1->Q);

    if (PostCount++ % THREAD_GC_ROUND == 0) {
        Allocator.GC();
    }

    return {0};
}

void *Thread::ThreadProcess(void *Argument) {

    Queue *Q;
    Job *J;

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
                Q->Detach();

                J = Job::FromQueue(Q);
                J->doJob();

                T->Destroy(J);
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
        Threads.push_back(new Thread());
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

