
class HTTPListening : public TCPListening {
protected:

    HTTPServer *ParentServer;
    SocketEventDomain *ParentEventDomain;

    friend class HTTPServer;

    virtual RuntimeError HandleDomainEvent(EventType Type);

public:

    HTTPListening(Address_t &Address);

    ~HTTPListening() = default;
};
