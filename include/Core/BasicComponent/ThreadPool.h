
class Thread;

class ThreadPool;

typedef void (ThreadFn)(void *PointerToArguments);

class Job {

private:

    Queue Q;

protected:

    void *PointerToArg = nullptr;
    ThreadFn *Callback = nullptr;

    friend class Thread;

public:

    Job() = default;
    Job(ThreadFn *Fn, void *Argument);
    Job(Job &J);

    static inline Job *FromQueue(Queue *Q) {
        return (Job *)((uintptr_t)Q - (uintptr_t)&(((Job*)0)->Q));
    }

    void doJob();
};

class Thread: public AllocatorBuild<Job> {

private:

    pthread_t WorkerThread;
    Mutex Lock;
    Queue Sentinel;
    Pool Allocator;
    bool Running;
    uint32_t PostCount;

    static void* ThreadProcess(void *Argument);

public:
    Thread();

    ~Thread() = default;

    void Stop();

    RuntimeError PostJob(Job &J);

};

class ThreadPool {

private:
    int NumThread;
    int DeliverIndex = 0;
    std::vector<Thread *> Threads;

public:
    ThreadPool(int NumThread = 8);

    ~ThreadPool();

    Thread *GetOneThread();
};
