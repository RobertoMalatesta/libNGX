namespace ngx::Core {

    class Server {
        private:
            static void EmptyPromise(void *Arguments, ThreadPool *TPool);
        protected:
            Listening ListeningSentinel;
            Connection ConnectionSentinel;
            SpinLock Lock;
            atomic_uint64_t MaxConnection = {0};
            PromiseCallback *ProcessFinished;
            PromiseCallback *NewConnection;
            PromiseCallback *ConnectionRead;
            PromiseCallback *ConnectionWrite;
        public:
            Server();
            EventError EnqueueListening(Listening *L);
            Listening *DequeueListening();
            EventError AttachConnection(Connection *C);
            EventError DetachConnection(Connection *C);

            virtual RuntimeError PostProcessFinished(void *Arguments);
            virtual RuntimeError PostNewConnection(void *Arguments);
            virtual RuntimeError PostConnectionRead(void *Arguments);
            virtual RuntimeError PostConnectionWrite(void *Arguments);

    };
}