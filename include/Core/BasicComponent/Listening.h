class Listening : public Socket, public Queue {
public:
    Listening();

    Listening(struct SocketAddress &SocketAddress);

    Listening(int SocketFD, struct SocketAddress &SocketAddress);

    virtual SocketError Listen() {
        return {ENOENT, "method not implement"};
    };

    virtual SocketError Close() {
        return {ENOENT, "method not implement"};
    };
};

class TCP4Listening : public Listening {
protected:
    uint Backlog = 1024;
public:
    TCP4Listening(struct SocketAddress &SocketAddress);

    ~TCP4Listening();

    virtual SocketError Listen();
    virtual SocketError Close();

    SocketError SetPortReuse(bool Open);
};

