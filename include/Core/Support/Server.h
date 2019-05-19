class Server {
protected:
    spin_lock Lock;
    EventDomain *Domain;

public:
    Server():Domain(nullptr), Lock() {};
    // Server Main EventLoop
    virtual RuntimeError ServerEventLoop() = 0;
};
