const EventType  ET_NONE = 0x0000;
const EventType  ET_TIMER = 0x0001;
const EventType  ET_CONNECTED = 0x0002;
const EventType  ET_READ = 0x0004;
const EventType  ET_WRITE = 0x0008;
const EventType  ET_CLOSED = 0x0010;

class EventDomain {
protected:
    SpinLock Lock;
    Pool Allocator;
    TimerTree Timers;
    ThreadPool TPool;
public:
    EventDomain(size_t PoolSize, int ThreadCount);

    ~EventDomain() {
        SpinlockGuard LockGuard(&Lock);
        TPool.~ThreadPool();
        Timers.~TimerTree();
    }

    static void DiscardPromise(void *Argument, ThreadPool *TPool) {};

    inline RuntimeError SetTimer(Timer &Timer, uint64_t Interval, TimerMode Mode) {

        ResetTimer(Timer);

        SpinlockGuard LockGuard(&Lock);
        Timer.SeInterval(Interval, Mode);
        Timers.AttachTimer(Timer);
        return {0};
    }

    inline RuntimeError ResetTimer(Timer &Timer) {
        SpinlockGuard LockGuard(&Lock);
        Timers.DetachTimer(Timer);
        Timer.Mode = TM_CLOSED, Timer.Interval = 0;
        return {0};
    }

    RuntimeError PostPromise(PromiseCallback *Callback, void *Argument);

    RuntimeError QueueExpiredTimer();

    virtual RuntimeError EventDomainProcess();
};
