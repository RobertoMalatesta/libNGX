#ifndef NGX_HTTP_CONNECTION
#define NGX_HTTP_CONNECTION

#include "Core/Core.h"
#include "HTTP/HTTPContext.h"
namespace ngx {
    namespace HTTP {

        using namespace ngx::Core::Support;

        class HTTPConnection : public TCPConnection {
        protected:
            Timer TNode;
            EventDomain *Domain;
            Pool connectionPool;
            std::unique_ptr<WritableMemoryBuffer> headerBuffer, bodyBuffer;
            std::unique_ptr<WritableMemoryBuffer> responseBuffer;

            static void HttpConnectionHandle(void *pListen, void *pArg);

            static HTTPConnection *buildHTTPConnection(HTTPContext &Context);

        public:
            HTTPConnection() : TCPConnection() {
                Fn = HttpConnectionHandle;
            }
            HTTPConnection(int FD, Address_t &Addr) : TCPConnection(FD, Addr) {
                Fn = HttpConnectionHandle;
            }

            ~HTTPConnection() = default;

            SocketError close();

            virtual void reset();

            virtual RuntimeError bindDomain(EventDomain &D);

            virtual RuntimeError unbindDomain();
        };
    }
}
#endif