#include "Core/Support/Support.h"
#ifndef NGX_HTTP_LISTEN
#define NGX_HTTP_LISTEN
namespace ngx {
    namespace HTTP {
        using namespace Core;
        using namespace Support;
        class HTTPListen : public TCPListen {
        protected:
            Timer TNode;
            EventDomain *Domain;
            static void HttpListenHandle(void *pListen, void *Event);

        public:
            HTTPListen() : TCPListen(), Domain(nullptr) {
                Fn = &HttpListenHandle;
            };
            HTTPListen(int FD, Address_t &Addr) : TCPListen(FD, Addr), Domain(nullptr) {
                Fn = &HttpListenHandle;
            };
            ~HTTPListen() = default;
            virtual RuntimeError bindDomain(EventDomain &D);
            virtual RuntimeError unbindDomain();
        };
    }
}
#endif