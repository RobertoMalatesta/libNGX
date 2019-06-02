#include "Core/Support/ThreadPool.h"
#include <unistd.h>

using namespace ngx::Core::Support;

Job::Job(ThreadFn *Fn, void *pObj, void *pArg) : Callback(Fn), pObj(pObj), pArg(pArg) {}

Job::Job(Job &J) : Callback(J.Callback), pObj(J.pObj), pArg(J.pArg) {}

void Job::doJob() {
    if (Callback != nullptr && pObj != nullptr) {
        Callback(pObj, pArg);
    }
}

Thread::Thread() : Sentinel(),
                   PostCount(0),
                   Running(true),
                   PressureScore(0),
                   Worker(nullptr), ThreadLocalPool() {}

void Thread::newJob(Job *&J) {
    void *pMem;

    pMem = ThreadLocalPool.allocate(sizeof(Job));

    if (pMem != nullptr) {
        J = new(pMem) Job();
    } else {
        J = nullptr;
    }
}

void Thread::deleteJob(Job *&J) {
    if (J != nullptr) {
        ThreadLocalPool.free(reinterpret_cast<void *&>(J));
        J = nullptr;
    }
}

RuntimeError Thread::postJob(const Job &J) {

    Job *J1 = nullptr;
    std::lock_guard<spin_lock> g(Lock);

    if (!Running) {
        return {EFAULT, "thread is not running"};
    }

    newJob(J1);

    if (J1 == nullptr) {
        return {ENOMEM, "insuffcient memory"};
    }

    *J1 = J;

    J1->appendJob(Sentinel);
    PressureScore++;

    if (PostCount++ % THREAD_COLLECT_ROUND == 0) { ThreadLocalPool.collect(); }

    return {0};
}

void *Thread::threadProcess(void *Arg) {

    Job *J;
    qnode *Q;
    Thread *thisThread;

    thisThread = static_cast<Thread *>(Arg);

    while (true) {

        ForceSleep(THREAD_WAIT_TIME);
        thisThread->Lock.lock();

        if (thisThread->Running) {
            while (!thisThread->Sentinel.isEmpty()) {
                // fetch one job from job list
                Q = thisThread->Sentinel.next();
                Q->detach();
                thisThread->Lock.unlock();
                // release thread lock and execute job
                J = Job::from_qnode(Q);
                J->doJob();
                thisThread->Lock.lock();
                // lock thread and free job object from ThreadLocalPool
                thisThread->deleteJob(J);
                thisThread->PressureScore--;
            }
            thisThread->Lock.unlock();
        } else {
            break;
        }
    }
    thisThread->Lock.unlock();
    return nullptr;
}

void Thread::start() {
    Running = true, PostCount = 0;
    ThreadLocalPool.collect();
    Worker = new std::thread(Thread::threadProcess, static_cast<void *>(this));
}

void Thread::stop() {
    {
        std::lock_guard<spin_lock> g(Lock);
        Running = false;
    }
    Worker->join();
}

ThreadPool::ThreadPool(int NumThread) : NumThread(NumThread) {
    while (NumThread-- > 0) {
        Threads.push_back(new Thread());
    }

    for (Thread *Temp: Threads) {
        Temp->start();
    }
}

ThreadPool::~ThreadPool() {
    for (Thread *Temp: Threads) {
        Temp->stop(), delete Temp;
    }
    Threads.clear();
}

Thread *ThreadPool::fetchOneThread() {
    Thread *Ret = nullptr;
    uint32_t PressureScore = UINT32_MAX;

    for (Thread *T: Threads) {
        if (T->presureScore() < PressureScore) {
            Ret = T, PressureScore = T->presureScore();
        }
    }
    return Ret;
}

