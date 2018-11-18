
class Server {
protected:
    SpinLock Lock;
    Listening ListeningSentinel;
    Connection ConnectionSentinel;
    std::atomic_uint64_t MaxConnection = {1024};

public:
    Server();

    EventError EnqueueListening(Listening *L);

    Listening *DequeueListening();

    EventError AttachConnection(Connection &C);

    EventError DetachConnection(Connection &C);

    virtual RuntimeError PostProcessFinished() = 0;
};
