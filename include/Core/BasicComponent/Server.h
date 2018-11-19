
class Server {
protected:
    SpinLock Lock;

public:
    Server() = default;

    virtual RuntimeError PostProcessFinished() = 0;

    virtual RuntimeError ServerEventProcess() = 0;
};
