class HTTPConnection : public TCPConnection {
protected:
    Timer TNode;
    Pool MemPool;
    EventDomain *Domain;
    std::unique_ptr<WritableMemoryBuffer> HeaderPart, ContentPart;

    Buffer ReadBuffer;
    HTTPRequest Request;

    static void HttpConnectionHandle(void *pListen, void *pArg);

    friend class HTTPConnectionBuilder;

public:
    HTTPConnection() : TCPConnection() {
        Fn = HttpConnectionHandle;
    }
    HTTPConnection(int FD, Address_t &Addr) : TCPConnection(FD, Addr) {
        Fn = HttpConnectionHandle;
    }

    ~HTTPConnection() = default;

    SocketError close();

    virtual void reset();

    virtual RuntimeError bindDomain(EventDomain &D);

    virtual RuntimeError unbindDomain();
};


