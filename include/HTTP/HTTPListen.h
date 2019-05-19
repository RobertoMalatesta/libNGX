
class HTTPListen : public TCPListen {
protected:
    Timer TNode;
    EventDomain *Domain;

    friend class HTTPServer;

    static void HttpListenHandle(void *pListen, void *Event);

public:
    HTTPListen() : TCPListen(), Domain(nullptr) {
        Fn = &HttpListenHandle;
    };

    HTTPListen(int FD, Address_t &Addr) : TCPListen(FD, Addr), Domain(nullptr) {
        Fn = &HttpListenHandle;
    };

    ~HTTPListen() = default;

    virtual RuntimeError bindDomain(EventDomain &D);

    virtual RuntimeError unbindDomain();
};
