const EventType  ET_NONE = 0x0000;
const EventType  ET_TIMER = 0x0001;
const EventType  ET_CONNECTED = 0x0002;
const EventType  ET_READ = 0x0004;
const EventType  ET_WRITE = 0x0008;
const EventType  ET_CLOSED = 0x0010;

class EventDomain {
protected:
    Spinlock Lock;
    Pool Allocator;
    TimerTree Timers;
    ThreadPool TPool;
public:
    EventDomain(size_t PoolSize, int ThreadCount);

    ~EventDomain() {
        LockGuard LockGuard(&Lock);
        TPool.~ThreadPool();
        Timers.~TimerTree();
    }

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {};

    inline RuntimeError SetTimer(EventEntity &Entity, uint64_t Interval, TimerMode Mode) {
        LockGuard LockGuard(&Lock);
        Timers.DetachTimer(Entity.TimerNode);
        Entity.TimerNode.SeInterval(Interval, Mode);
        Timers.AttachTimer(Entity.TimerNode);
        return {0};
    }

    inline RuntimeError ResetTimer(EventEntity &Entity) {
        LockGuard LockGuard(&Lock);
        Entity.Lock.Lock();
        Timers.DetachTimer(Entity.TimerNode);
        Entity.TimerNode.Mode = TM_CLOSED, Entity.TimerNode.Interval = 0;
        Entity.Lock.Unlock();
        return {0};
    }

    RuntimeError PostPromise(PromiseCallback *Callback, void *Argument);

    RuntimeError QueueExpiredTimer();

    virtual RuntimeError EventDomainProcess();
};
