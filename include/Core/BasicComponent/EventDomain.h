typedef uint32_t EventType;

const uint32_t ET_NONE = 0x0000;
const uint32_t ET_TIMER = 0x0001;
const uint32_t ET_CONNECTED = 0x0002;
const uint32_t ET_READ = 0x0004;
const uint32_t ET_WRITE = 0x0008;
const uint32_t ET_CLOSED = 0x0010;

typedef enum {
    SOCK_READ_EVENT = 0,
    SOCK_WRITE_EVENT,
    SOCK_READ_WRITE_EVENT,
    SOCK_TIMER_EVENT,
} SocketEventType;


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
        SpinlockGuard LockGuard(&Lock);
        Timers.DetachTimer(Timer);
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

    virtual RuntimeError EventDomainProcess(EventPromiseArgs &Argument);
};
