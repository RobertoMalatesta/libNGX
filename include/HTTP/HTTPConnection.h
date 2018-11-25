
class HTTPConnection : public TCP4Connection, public Recyclable {
protected:
    Pool MemPool;

    Buffer ReadBuffer;
    HTTPRequest Request;

    HTTPServer *ParentServer = nullptr;
    SocketEventDomain *ParentEventDomain = nullptr;


    virtual RuntimeError HandleEventDomain(uint32_t EventType);

    static void OnTimerEvent(void *PointerToConnection, ThreadPool *TPool);

    static void OnConnectionEvent(void *PointerToConnection, ThreadPool *TPool);

    friend class HTTPListening;

    friend class HTTPConnectionBuilder;

public:

    HTTPConnection();

    ~HTTPConnection() = default;

    inline RuntimeError ReadConnection() {
        return ReadBuffer.ReadFromConnection(this);
    }

    inline RuntimeError WriteConnection() {
        return RuntimeError(EINVAL);
    }

    RuntimeError ProcessRequest() {
        return RuntimeError(0);
    };

    RuntimeError SetSocketAddress(int SocketFD, struct SocketAddress &Address);

    SocketError Close();

    virtual void Reset();
};


