namespace ngx::Core {

    const int EPOLL_EVENT_BATCH_SIZE = 512;
    const int EPOLL_EVENT_WAIT_TIME = 10;
    const int EPOLL_EVENT_MAX_CONNECTION = 32768;


    class EPollEventDomain : public EventDomain {
        protected:
            int EPollFD = -1;
            atomic_flag Waiting = ATOMIC_FLAG_INIT;
            atomic_int64_t MaxConnection = EPOLL_EVENT_MAX_CONNECTION;
            SpinLock ModifyLock;
            Listening ListenSentinel;
            Connection ConnectionSentinel;
            static void EPollEventProcessPromise(void* Args, ThreadPool *TPool);

        public:

            EPollEventDomain(size_t PoolSize, int ThreadCount, int EPollSize);
            ~EPollEventDomain();

            EventError EPollAttachSocket(Socket *S);
            EventError EPollDetachSocket(Socket *S);
            EventError EPollAddListening(Listening *L);
            EventError EPollRemoveListening(Listening *L);
            EventError EPollEnqueueListening(Listening *L);
            Listening *EPollDequeueListening();
            EventError EPollAddConnection(Connection *C);
            EventError EPollRemoveConnection(Connection *C);
            RuntimeError EPollListenToNext();
            RuntimeError EventDomainProcess();
    };
}