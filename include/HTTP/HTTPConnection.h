class HTTPConnection : public TCPConnection {
protected:

    Job EventJob;
    Pool MemPool;

    HTTPServer *ParentServer = nullptr;
    SocketEventDomain *ParentEventDomain = nullptr;

    std::unique_ptr<WritableMemoryBuffer> HeaderPart, ContentPart;

    Buffer ReadBuffer;
    HTTPRequest Request;

    virtual RuntimeError HandleDomainEvent(EventType Type);

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
};


