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
        public:
            Server();
            EventError EnqueueListening(Listening *L);
            Listening *DequeueListening();
            EventError AttachConnection(Connection *C);
            EventError DetachConnection(Connection *C);
    };
}