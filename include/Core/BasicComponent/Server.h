
class Server {
protected:
    Spinlock Lock;

public:
    Server() = default;

    virtual RuntimeError PostProcessFinished() = 0;

    virtual RuntimeError ServerEventProcess() = 0;
};
