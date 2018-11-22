class Listening : public Socket {
public:
    Listening();

    Listening(struct SocketAddress &SocketAddress);

    Listening(int SocketFD, struct SocketAddress &SocketAddress);

    virtual SocketError Bind() = 0;

    virtual SocketError Listen() = 0;

    virtual SocketError Close() = 0;
};

class TCP4Listening : public Listening {
protected:
    uint Backlog = 32768;
public:
    TCP4Listening(struct SocketAddress &SocketAddress);

    ~TCP4Listening();

    virtual SocketError Bind();

    virtual SocketError Listen();

    virtual SocketError Close();

    virtual RuntimeError HandleEventDomain(EventType Type) {
        return {0};
    };

    SocketError SetPortReuse(bool Open);
};

