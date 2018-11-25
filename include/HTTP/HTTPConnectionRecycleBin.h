
class HTTPConnectionRecycleBin : public RecycleBin, public AllocatorBuild<HTTPConnection> {
protected:
    uint32_t AllocateCount = 0;
    Pool BackendAllocator;
public:
    HTTPConnectionRecycleBin(uint64_t RecycleBinSize);

    ~HTTPConnectionRecycleBin();

    virtual int Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress);

    virtual int Put(HTTPConnection *&Item);
};
