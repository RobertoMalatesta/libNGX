namespace ngx::Core {

    typedef enum {
        NONE_EVENT = 0x00,
        TIMER_EVENT = 010,

        CONNECTION_READ =0x20,
        CONNECTION_WRITE = 0x21,
    } EventType;

    class Event {
        public:
            virtual EventType GetType() {return NONE_EVENT;};
    };

    class EventDomain {
        private:
            Pool Allocator;
            ThreadPool *TPool;
            TimerTree *Timers;

        public:

            EventDomain(size_t PoolSize, int ThreadCount);
            ~EventDomain();

            void PostTimerEvent(uint32_t Timestamp, PromiseCallback *Callback, void *Argument);
            void EventDomainProcess();
    };
}