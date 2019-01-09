class HTTPConnection : public TCPConnection, public Reusable {
protected:

    Job EventJob;
    Pool MemPool;

    HTTPServer *ParentServer = nullptr;
    SocketEventDomain *ParentEventDomain = nullptr;

    Buffer ReadBuffer;
    HTTPRequest Request;

    virtual RuntimeError HandleEventDomain(uint32_t EventType);

    static void OnTimerEvent(void *PointerToConnection);

    static void OnConnectionEvent(void *PointerToConnection);

    friend class HTTPListening;

    friend class HTTPConnectionBuilder;

public:

    HTTPConnection();

    ~HTTPConnection() = default;

    inline RuntimeError ReadConnection() {
        return ReadBuffer.ReadConnection(this);
    }

    inline RuntimeError WriteConnection() {
        return RuntimeError(EINVAL);
    }

    RuntimeError ProcessRequest() {
        return RuntimeError(0);
    };


    SocketError Close();

    virtual void Reset();

    static inline HTTPConnection *FromCollectQueue(Queue *Q) {

        if (Q == nullptr) {
            return nullptr;
        }
        return (HTTPConnection *) ((uintptr_t) Q - (uintptr_t) (&((HTTPConnection *) 0)->ReuseItem));
    }
};


