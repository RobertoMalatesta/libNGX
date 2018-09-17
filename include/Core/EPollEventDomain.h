namespace ngx::Core {

    const int EVENT_BATCH_SIZE = 512;
    const int EVENT_WAIT_TIME = 10;

    class EPollEventDomain : public EventDomain {
        private:
            int EPollFD = -1;
            atomic_flag Waiting = ATOMIC_FLAG_INIT;
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
            EventError EPollAddConnection(Connection *C);
            EventError EPollRemoveConnection(Connection *C);
            RuntimeError EPollListenToNext();
            RuntimeError EventDomainProcess();
    };
}