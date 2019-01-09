
class HTTPConnectionCollector : public Collector, public AllocatorBuild<HTTPConnection>, public CanReset {
protected:
    uint32_t AllocateCount = 0;
    Pool BackendAllocator;
public:
    HTTPConnectionCollector(uint32_t CollectorSize);

    ~HTTPConnectionCollector();

    virtual int Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress);

    virtual int Put(HTTPConnection *&Item);

    virtual void Reset();
};
