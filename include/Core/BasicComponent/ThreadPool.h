
class Thread;

class ThreadPool;

typedef void (PromiseCallback)(void *PointerToArguments, ThreadPool *TPool);

class Promise {
private:

    Queue Q;
    ThreadPool *TPool = nullptr;

protected:

    void *PointerToArg = nullptr;
    PromiseCallback *Callback = nullptr;

    friend class Thread;
    friend class ThreadPool;

public:
    Promise();

    static inline Promise *FromQueue(Queue *Q) {
        return (Promise *)((uintptr_t)Q - (uintptr_t)&(((Promise*)0)->Q));
    }

    void doPromise();
};

class Thread: public AllocatorBuild<Promise> {

private:

    ThreadPool *TPool = nullptr;
    pthread_t WorkerThread;
    Mutex Lock;
    Queue Sentinel;
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
