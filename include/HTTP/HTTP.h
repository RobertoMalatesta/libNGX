#include "Core/Core.h"

namespace ngx {
    namespace HTTP {

        const char LF = '\n';
        const char CR = '\r';
        const char CRLF[] = "\r\n";

        using namespace ngx::Core;

        class HTTPError;

        class HTTPServer;

        class HTTPConnection;

        class HTTPConnectionRecycler;

        class HTTPParser;

        class HTTPRequest;

        struct HTTPPerformanceUnit;

        enum HTTPVersion {
            HTTP_V1 = 1,
            HTTP_V1_1,
            HTTP_V2,
        };

        enum HTTPMethod {
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

        enum HTTPRequestState {
            HTTP_BAD_REQUEST_STATE = -1,
            HTTP_INIT_STATE,
            HTTP_PAESE_METHOD,
            HTTP_PARSE_REQUEST_LINE,
        };
    }
}

#include "HTTP/HTTPError.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPParser.h"
#include "HTTP/HTTPConnection.h"
#include "HTTP/HTTPConnectionRecycler.h"
#include "HTTP/HTTPConnectionBuilder.h"
#include "HTTP/HTTPServer.h"
