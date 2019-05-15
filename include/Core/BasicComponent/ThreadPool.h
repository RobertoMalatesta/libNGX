class Job;
class ThreadPool;

typedef void (ThreadFn)(void *pArg);

class Job {
private:
    Queue Q;

protected:
    void *pArg = nullptr;
    ThreadFn *Callback = nullptr;

public:

    Job() = default;

    Job(ThreadFn *Fn, void *pArg);

    Job(Job &J);

    static inline Job *FromQueue(Queue *Q) {
        return (Job *) ((uintptr_t) Q - (uintptr_t) &(((Job *) 0)->Q));
    }

    inline void AppendJob(Queue &queue) { queue.Append(&Q); };
    void doJob();
};

class ThreadPool {
public:
    class Thread {
    private:
        SpinLock Lock;
        Queue Sentinel;
        Pool ThreadLocalPool;

        std::thread *Worker;

        bool Running;
        uint32_t PostCount, PressureScore;

        // create and destory job from ThreadLocalPool
        void newJob(Job *&J);
        void deleteJob(Job *&J);
        // thread backend routine, process jobs
        static void *ThreadProcess(void *Arg);

    public:
        Thread();
        ~Thread() = default;
        // control thread running state
        void Start();
        void Stop();
        // post a new job to the job queue
        RuntimeError PostJob(Job &J);
        uint32_t PresureScore() const {return PressureScore; };
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
