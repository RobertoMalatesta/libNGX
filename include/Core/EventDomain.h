namespace ngx::Core {

    typedef enum {
        NONE_EVENT = 0x00,
        TIMER_EVENT = 010,
        CONNECTION_READ =0x20,
        CONNECTION_WRITE = 0x21,
    } EventType;

    struct SocketEventDomainArgument {
        size_t ArgumentSize;
        SocketEventDomain *EventDomain;
        Listening *Listening;
    };

    class Event {
        public:
            virtual EventType GetType() {return NONE_EVENT;};
    };

    class EventDomain {
        protected:
            Pool Allocator;
            ThreadPool TPool;
            TimerTree *Timers;
            SpinLock Lock;
        public:
            EventDomain(size_t PoolSize, int ThreadCount);
            ~EventDomain();
            static void DiscardPromise(void *Argument, ThreadPool *TPool) {};
            RuntimeError PostTimerEvent(uint32_t Timestamp, PromiseCallback *Callback, void *Argument);
            RuntimeError EventDomainProcess(void *PointerToArgument);
    };

    typedef enum {
        SOCK_READ_EVENT = 0,
        SOCK_WRITE_EVENT,
        SOCK_READ_WRITE_EVENT
    } SocketEventType;

    class SocketEventDomain : public EventDomain {
        protected:
            bool IsSocketReadAttached(Socket *S) {

                if (nullptr == S) {
                    return false;
                }

                return S->ReadAttach == 1;
            }

            bool IsSocketWriteAttached(Socket *S) {
                if (nullptr == S) {
                    return false;
                }
                return S->WriteAttach == 1;
            }

            void SetSocketReadAttached(Socket *S, int Val) {
                if (nullptr == S) {
                    return;
                }
                S->ReadAttach = (Val == 1)? 1: 0;
                return;
            }

            void SetSocketWriteAttached(Socket *S, int Val) {
                if (nullptr == S) {
                    return;
                }
                S->WriteAttach = (Val == 1)? 1: 0;
                return;
            }
        public:
            SocketEventDomain(size_t PoolSize, int ThreadCount);
            EventError AttachSocket(Socket *S, SocketEventType Type) { return EventError(0);};
            EventError DetachSocket(Socket *S, SocketEventType Type) { return EventError(0);};
    };
}