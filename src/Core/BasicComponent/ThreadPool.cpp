#include "Core/Core.h"
#include <unistd.h>

using namespace ngx::Core::BasicComponent;

Job::Job(ThreadFn *Fn, void *pArg) : Callback(Fn), pArg(pArg) {
}

Job::Job(Job &J) : Callback(J.Callback), pArg(J.pArg) {
}

void Job::doJob() {
    if (Callback != nullptr) {
        Callback(pArg);
    }
}

Thread::Thread() :  Sentinel(),
                    PostCount(0),
                    Running(true),
                    PressureScore(0),
                    Worker(nullptr),ThreadLocalPool(){}

void Thread::newJob(Job *&J) {
    void *pMem;

    pMem=ThreadLocalPool.allocate(sizeof(Job));

    if(pMem!=nullptr) {
        J = new(pMem) Job();
    } else {
        J = nullptr;
    }
}

void Thread::deleteJob(Job *&J) {
    if(J != nullptr) {
        ThreadLocalPool.free(reinterpret_cast<void *&>(J));
        J= nullptr;
    }
}

RuntimeError Thread::PostJob(Job &J) {

    Job *J1 = nullptr;

    LockGuard Guard(&Lock);

    if (!Running) {
        return {EFAULT, "thread is not running"};
    }

    newJob(J1);

    if (J1 == nullptr) {
        return {ENOMEM, "insufficent memory"};
    }

    *J1 = J;

    J1->AppendJob(Sentinel);
    PressureScore++;

    if (PostCount++ % THREAD_COLLECT_ROUND == 0) { ThreadLocalPool.collect(); }

    return {0};
}

void *Thread::ThreadProcess(void *Arg) {

    Job *J;
    Queue *Q;
    Thread *thisThread;

    thisThread = static_cast<Thread *>(Arg);

    while (true) {

        ForceSleep(THREAD_WAIT_TIME);
        thisThread->Lock.Lock();

        if (thisThread->Running) {
            while (!thisThread->Sentinel.IsEmpty()) {
                // fetch one job from job list
                Q = thisThread->Sentinel.GetNext();
                Q->Detach();
                thisThread->Lock.Unlock();
                // release thread lock and execute job
                J = Job::FromQueue(Q);
                J->doJob();
                thisThread->Lock.Lock();
                // lock thread and free job object from ThreadLocalPool
                thisThread->deleteJob(J);
                thisThread->PressureScore--;
            }
            thisThread->Lock.Unlock();
        } else {
            break;
        }
    }
    thisThread->Lock.Unlock();
    return nullptr;
}

void Thread::Start() {
    Running = true, PostCount = 0;
    ThreadLocalPool.collect();
    Worker = new std::thread(Thread::ThreadProcess, static_cast<void *>(this));
}

void Thread::Stop() {
    {
        LockGuard LockGuard(&Lock);
        Running = false;
    }
    Worker->join();
}

ThreadPool::ThreadPool(int NumThread) : NumThread(NumThread) {
    while (NumThread-- > 0) {
        Threads.push_back(new Thread());
    }

    for (Thread *Temp: Threads) {
        Temp->Start();
    }
}

ThreadPool::~ThreadPool() {
    for (Thread *Temp: Threads) {
        Temp->Stop(), delete Temp;
    }
    Threads.clear();
}

Thread *ThreadPool::fetchOneThread() {
    Thread *Ret= nullptr;
    uint32_t PressureScore = UINT32_MAX;

    for(Thread * T: Threads) {
        if(T->PresureScore() < PressureScore) {
            Ret = T, PressureScore = T->PresureScore();
        }
    }

    return Ret;
}

