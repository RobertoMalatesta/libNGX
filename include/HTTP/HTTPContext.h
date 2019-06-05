#include "HTTP/common_header.h"
#include "HTTP/HTTPConnection.h"
#include "HTTP/HTTPComponent.h"
#ifndef LIBNGX_HTTP_CONTEXT
#define LIBNGX_HTTP_CONTEXT
namespace ngx {
    namespace HTTP {
        using namespace ngx::Core;
        class HTTPContext {
//          HTTP server context
//          client header size
            size_t clientHeaderSize = 2 * KB;
//          maximum client header size
            size_t maxClientHeaderSize = 16 * KB;
//          maximum client body size
            size_t maxClientBodySize = 16 * KB;
//          backward proxy connect timeout
            time_t connectTimeout = 5 * s;
//          backward proxy read timeout
            time_t readTimeout = 10 * s;
//          connection keep alive timeout
            time_t keepaliveTimeout = 10 * min;
//          maximum keepalive requests
            time_t keepaliveRequests = -1;
//          event moudle
            std::string eventMoudle = "epoll";

//          environment variables
            std::map<std::string, std::string> env;
//          backend components
//            std::map<int, Component> components;
//          current request mux
//            std::unique_ptr<Component> mux;

//          Performance Mointor
//          eventLoop() process count
            std::atomic_uint64_t eventLoopCount = ATOMIC_VAR_INIT(0);
//          current connection count
            std::atomic_uint64_t liveConnectionCount = ATOMIC_VAR_INIT(0);
//          serverd connection count
            std::atomic_uint64_t totalConnectionCount = ATOMIC_VAR_INIT(0);
//          request processed count
            std::atomic_uint64_t requestCount = ATOMIC_VAR_INIT(0);
//          requests with normal response code
            std::atomic_uint64_t normalResponseCount = ATOMIC_VAR_INIT(0);
//          requests with error response code
            std::atomic_uint64_t errorResponseCount = ATOMIC_VAR_INIT(0);
//          read() count
            std::atomic_uint64_t readCount = ATOMIC_VAR_INIT(0);
//          read() bytes
            std::atomic_uint64_t readBytes = ATOMIC_VAR_INIT(0);
//          write() count
            std::atomic_uint64_t writeCount = ATOMIC_VAR_INIT(0);
//          write() bytes
            std::atomic_uint64_t writeBytes = ATOMIC_VAR_INIT(0);
        };
    }
}
#endif