
struct HTTPPMU {

    // EventLoop() process count
    std::atomic_uint64_t ProcessRound = {0};

    // Live connection count
    std::atomic_uint64_t ConnectionCount = {0};

    // Processed connection count(including closed connection)
    std::atomic_uint64_t HistoryConnecionCount = {0};

    //Request processed count
    std::atomic_uint64_t RequestCount = {0};

    // Requests with normal response code
    std::atomic_uint64_t NormalClosedCount = {0};

    // Requests with error response code
    std::atomic_uint64_t ErrorClosedCount = {0};

    // read() count
    std::atomic_uint64_t ReadCount = {0};

    // read() bytes
    std::atomic_uint64_t ReadBytes = {0};

    // write() count
    std::atomic_uint64_t WriteCount = {0};

    // write() bytes
    std::atomic_uint64_t WriteBytes = {0};

    // waiting connections count
    std::atomic_uint64_t WatingCount = {0};
};

class HTTPServer : public Server {

protected:

    //HTTP Performance Counter
    HTTPPMU PMU;

    //HTTP Connection Builder
    HTTPConnectionBuilder ConnectionBuilder;

    virtual RuntimeError OnLoopFinished();

public:

    HTTPServer(size_t BufferBlockSize,
               uint32_t ConnectionCollectorSize,
               uint32_t BufferCollectorSize,
               SocketEventDomain *EventDomain);

    // Attach a listening to this server
    virtual EventError AttachListening(HTTPListening &L);

    // Detach a listening from this server
    virtual EventError DetachListening(HTTPListening &L);

    // Get a new Connection from ConnectionBuilder
    virtual RuntimeError GetConnection(HTTPConnection *&C, int SocketFD, SocketAddress &Address);

    // Put a connection back into ConnectionBuilder
    virtual RuntimeError PutConnection(HTTPConnection *&C);

    // Process HTTP event dispatch
    RuntimeError ServerEventLoop();
};
