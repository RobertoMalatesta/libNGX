namespace ngx {
    namespace Core {

        class Server {
        protected:
            Listening ListeningSentinel;
            Connection ConnectionSentinel;
            SpinLock Lock;
            std::atomic_uint64_t MaxConnection = {0};

        public:
            Server();

            EventError EnqueueListening(Listening *L);

            Listening *DequeueListening();

            EventError AttachConnection(Connection *C);

            EventError DetachConnection(Connection *C);

            virtual RuntimeError PostProcessFinished(EventPromiseArgs &Argument);

            virtual RuntimeError PostConnectionEvent(EventPromiseArgs &Argument);
        };
    }
}