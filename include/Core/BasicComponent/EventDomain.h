const EventType  ET_NONE = 0x0000;
const EventType  ET_TIMER = 0x0001;
const EventType  ET_ACCEPT = 0x0002;
const EventType  ET_READ = 0x0004;
const EventType  ET_WRITE = 0x0008;
const EventType  ET_CLOSED = 0x0010;

class EventDomain {
protected:
    ThreadPool TPool;
    SpinLock TPoolLock;
public:
    EventDomain(int ThreadCount);

    ~EventDomain() {
        LockGuard LockGuard(&TPoolLock);
        TPool.~ThreadPool();
    }

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {};

    RuntimeError BindEventThread(EventEntity &Entity);

    RuntimeError UnbindEventThread(EventEntity &Entity);

    virtual RuntimeError EventDomainProcess();
};
