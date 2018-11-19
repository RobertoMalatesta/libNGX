
class Thread;

class ThreadPool;

typedef void (PromiseCallback)(void *PointerToArguments, ThreadPool *TPool);

class Promise : public Queue {
private:
    void *PointerToArg = nullptr;
    ThreadPool *TPool = nullptr;
    PromiseCallback *Callback = nullptr;

    friend class Thread;

    friend class ThreadPool;

public:
    Promise();

    Promise(ThreadPool *TPool, Thread *T, PromiseCallback *Callback, void *PointerToArgs);

    void doPromise();
};

class Thread {

private:

    ThreadPool *TPool = nullptr;
    pthread_t WorkerThread;
    Mutex Lock;
    Promise Sentinel;
    Pool Allocator;
    bool Running;
    uint32_t PostCount;

    static void* ThreadProcess(void *Thread);

    friend Promise;

public:
    Thread(ThreadPool *TPool);

    ~Thread() = default;

    void Stop();

    int TryPostPromise(PromiseCallback *Callback, void *Argument);
};

class ThreadPool {

private:
    int NumThread;
    int DeliverIndex = 0;
    std::vector<Thread *> Threads;

    friend class Promise;

public:
    ThreadPool(int NumThread = 8);

    ~ThreadPool();

    void PostPromise(PromiseCallback *Callback, void *PointerToArg);
};
