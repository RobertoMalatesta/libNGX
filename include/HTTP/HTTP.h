#include "Core/Core.h"
#include "HTTP/config_header.h"

namespace ngx {
    namespace HTTP {

        const u_char LF = '\n';
        const u_char CR = '\r';
        const u_char CRLF[] = "\r\n";
        const bool DEFAULT_HTTP_ALLOW_UNDERSCORE = true;

        using namespace ngx::Core;
        using namespace ngx::HTTP::DefaultConfig;

        class HTTPError;

        class HTTPServer;

        class HTTPConnection;

        class HTTPConnectionRecycleBin;

        class HTTPConnectionBuilder;

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
            HTTP_PARSE_HEADER,
            HTTP_HEADER_DONE,
            HTTP_VALIDATE_URI,
            HTTP_VALIDATE_COMPLEX_URI,
            HTTP_READ_DATA,
            HTTP_READ_CHUNK,
        };

        enum HTTPResponseState {

        };
#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPParser.h"
#include "HTTP/HTTPListening.h"
#include "HTTP/HTTPConnection.h"
#include "HTTP/HTTPConnectionRecycleBin.h"
#include "HTTP/HTTPConnectionBuilder.h"
#include "HTTP/HTTPServer.h"
#include "HTTP/Router/Router.h"
    }
}

