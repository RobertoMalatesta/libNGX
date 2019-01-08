
class HTTPListening : public TCPListening {
protected:

    HTTPServer *ParentServer;
    SocketEventDomain *ParentEventDomain;

    friend class HTTPServer;

    virtual RuntimeError HandleEventDomain(uint32_t EventType);

public:

    HTTPListening(SocketAddress &Address);

    ~HTTPListening() = default;
};
