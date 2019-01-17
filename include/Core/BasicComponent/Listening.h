class Listening : public Socket {
protected:
    int Backlog = 32768;
public:
    Listening();

    Listening(SocketAddress &Address);

    Listening(int SocketFD, SocketAddress &Address);

    SocketError SetPortReuse(bool On);

    virtual SocketError Bind() = 0;

    virtual SocketError Listen();
};

class TCPListening : public Listening {
public:
    TCPListening(SocketAddress &Address);

    ~TCPListening();

    virtual SocketError Bind();

    virtual RuntimeError HandleDomainEvent(EventType Type) {
        return {0};
    };

};

