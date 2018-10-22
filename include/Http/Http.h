#include "Core/Core.h"

namespace ngx::Http {
    using namespace ngx::Core;

    class HttpError;

    class HttpServer;

    class HttpConnection;

    class HttpConnectionRecycler;

    class HttpConnectionBuilder;

    class HttpRequestContext;

    class HttpResponseBuffer;

    struct HttpPerformanceUnit;

    enum HttpVersion {
        HTTP_V1 = 1,
        HTTP_V1_1,
        HTTP_V2,
    };

    enum HttpMethod {
        GET,
        PUT,
        POST,
        COPY,
        MOVE,
        LOCK,
        HEAD,
        MKCOL,
        PATCH,
        TRACE,
        DELETE,
        UNLOCK,
        OPTIONS,
        PROPFIND,
        PROPPATCH,
    };
}

#include "Http/HttpError.h"
#include "Http/HttpConnection.h"
#include "Http/HttpConnectionRecyclerTest.h"
#include "Http/HttpServer.h"
#include "Http/HttpRequestContext.h"
