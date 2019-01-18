
class HTTPListening : public TCPListening {
protected:

    HTTPServer *ParentServer;
    SocketEventDomain *ParentEventDomain;

    friend class HTTPServer;

    virtual RuntimeError HandleDomainEvent(EventType Type);

public:

    HTTPListening(SocketAddress &Address);

    ~HTTPListening() = default;
};
