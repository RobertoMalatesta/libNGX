#include "Core/Support/Support.h"
#ifndef NGX_HTTP_CONNECTION
#define NGX_HTTP_CONNECTION
namespace ngx {
namespace HTTP {
class HTTPContext;
using namespace ngx::Core::Support;

class HTTPConnection : public TCPConnection {
protected:
    Timer TNode;
    EventDomain *Domain;
    Pool connectionPool;
    unique_ptr<WritableMemoryBuffer> headerBuffer, bodyBuffer;
    unique_ptr<BufferWriter> headerWriter, bodyWriter;
    static void HttpConnectionHandle(void *pListen, void *pArg);
public:
    HTTPConnection() : TCPConnection() {
        headerWriter=unique_ptr<BufferWriter>(new BufferWriter(*headerBuffer));
        Fn = HttpConnectionHandle;
    }
    HTTPConnection(int FD, Address_t &Addr) : TCPConnection(FD, Addr) {
        Fn = HttpConnectionHandle;
    }
    ~HTTPConnection() = default;
    SocketError close();
    RuntimeError bindDomain(EventDomain &D) final;
    RuntimeError unbindDomain() final;
    void reset();
};
} // namespace HTTP
} // namespace ngx
#endif