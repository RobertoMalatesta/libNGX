#include "Core/Core.h"

namespace ngx{
    namespace Http {
        const char LF = '\n';
        const char CR = '\r';
        const char CRLF[] = "\r\n";

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
}

#include "Http/HttpError.h"
#include "Http/HttpRequestContext.h"
#include "Http/HttpConnection.h"
#include "Http/HttpConnectionRecycler.h"
#include "Http/HttpServer.h"
