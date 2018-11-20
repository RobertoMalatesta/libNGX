const EventType  ET_NONE = 0x0000;
const EventType  ET_TIMER = 0x0001;
const EventType  ET_CONNECTED = 0x0002;
const EventType  ET_READ = 0x0004;
const EventType  ET_WRITE = 0x0008;
const EventType  ET_CLOSED = 0x0010;

class EventDomain {
protected:
    Pool Allocator;
    ThreadPool TPool;
    Spinlock Lock;
public:
    EventDomain(size_t PoolSize, int ThreadCount);

    ~EventDomain() {
        LockGuard LockGuard(&Lock);
        TPool.~ThreadPool();
    }

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {};

    RuntimeError PostPromise(PromiseCallback *Callback, void *Argument);

    virtual RuntimeError EventDomainProcess();
};
