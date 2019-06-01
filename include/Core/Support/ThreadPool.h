#include <vector>
#include <thread>
#include "Core/ADT/qnode.h"
#include "Core/Support/spin_lock.h"
#include "Core/Support/Pool.h"
#include "Core/Support/Time.h"
#include "Core/Support/Error.h"

#ifndef LIBNGX_CORE_SUPPORT_THREADPOOL
#define LIBNGX_CORE_SUPPORT_THREADPOOL

namespace ngx {
namespace Core {
namespace Support {

using namespace ADT;
class Job;
class ThreadPool;
typedef void (ThreadFn)(void *pObj, void *pArg);

class Job {
private:
    qnode Q;

protected:
    void *pObj;
    void *pArg;
    ThreadFn *Callback;

public:
    Job() : Callback(nullptr), pObj(nullptr), pArg(nullptr) {};
    Job(Job &J);
    Job(ThreadFn *Fn, void *pObj, void *pArg);
    inline void appendJob(qnode &queue) { queue.append(&Q); };
    void doJob();
    static inline Job *from_qnode(qnode *Q) { return (Job *) ((uintptr_t) Q - (uintptr_t) & (((Job *) 0)->Q)); }
};

class ThreadPool {
public:
    class Thread {
    private:
        spin_lock Lock;
        qnode Sentinel;
        Pool ThreadLocalPool;
        std::thread *Worker;
        bool Running;
        uint32_t PostCount, PressureScore;
        // create and destory job from ThreadLocalPool
        void newJob(Job *&J);
        void deleteJob(Job *&J);
        // thread backend routine, process jobs
        static void *threadProcess(void *Arg);

    public:
        Thread();
        ~Thread() = default;
        // control thread running state
        void start();
        void stop();
        // post a new job to the job queue
        RuntimeError postJob(const Job &J);
        uint32_t presureScore() const { return PressureScore; };
    };

    ThreadPool(int NumThread = 8);
    ~ThreadPool();
    // fetch one thread from thread pool
    Thread *fetchOneThread();

private:
    int NumThread;
    int DeliverIndex = 0;
    std::vector<Thread *> Threads;
};

using Thread=ThreadPool::Thread;

} // Support
} // Core
} // ngx


#endif