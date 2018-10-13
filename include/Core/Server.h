namespace ngx::Core {

    class Server {
        protected:
            Listening ListeningSentinel;
            Connection ConnectionSentinel;
            SpinLock Lock;
            atomic_uint64_t MaxConnection = {0};

        public:
            Server();
            EventError EnqueueListening(Listening *L);
            Listening *DequeueListening();
            EventError AttachConnection(Connection *C);
            EventError DetachConnection(Connection *C);

            virtual RuntimeError PostProcessFinished(EventPromiseArgs *Arguments);
            virtual RuntimeError PostNewConnection(EventPromiseArgs *Arguments);
            virtual RuntimeError PostConnectionRead(EventPromiseArgs *Arguments);
            virtual RuntimeError PostConnectionWrite(EventPromiseArgs *Arguments);
            virtual RuntimeError PostConnectionClosed(EventPromiseArgs *Arguments);
    };
}