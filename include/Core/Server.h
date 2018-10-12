namespace ngx::Core {

    struct ServerEventArgument : EPollEventDomainArgument{
        Server *Server;
    };

    class Server {
        protected:
            Listening ListeningSentinel;
            Connection ConnectionSentinel;
            SpinLock Lock;
            atomic_uint64_t MaxConnection = {0};
            PromiseCallback *ProcessFinished, *NewConnection, *ConnectionRead, *ConnectionWrite;
            static void OnProcessFinished(void *Arguments, ThreadPool *TPool) {};
            static void OnNewConnection(void *Arguments, ThreadPool *TPool) {};
            static void OnConnectionRead(void *Arguments, ThreadPool *TPool) {};
            static void OnConnectionWrite(void *Arguments, ThreadPool *TPool) {};
        public:
            Server();
            EventError EnqueueListening(Listening *L);
            Listening *DequeueListening();
            EventError AttachConnection(Connection *C);
            EventError DetachConnection(Connection *C);
            RuntimeError PostProcessFinished(void *Arguments, ThreadPool *TPool);
            RuntimeError PostNewConnection(void *Arguments, ThreadPool *TPool);
            RuntimeError PostConnectionRead(void *Arguments, ThreadPool *TPool);
            RuntimeError PostConnectionWrite(void *Arguments, ThreadPool *TPool);

    };
}