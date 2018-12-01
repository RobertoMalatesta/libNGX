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

        struct HTTPHeader {
            BoundCursor Name;
            BoundCursor Value;
            bool Valid;

            HTTPHeader() : Name(), Value(), Valid(true) {}
        };

        enum HTTPCoreHeaderIn {
            HI_COMMON = -1,
            HI_HOST = 0,
            HI_CONNECTION,
            HI_IF_MODIFY_SINCE,
            HI_IF_UNMODIFY_SINCE,
            HI_IF_MATCH,
            HI_IF_NON_MATCH,
            HI_USERAGENT,
            HI_REFERENCE,
            HI_CONTENT_LENGTH,
            HI_CONTENT_RANGE,
            HI_CONTENT_TYPE,
            HI_RANGE,
            HI_IF_RANGE,
            HI_TRANSFER_ENCODING,
            HI_TE,
            HI_EXPECT,
            HI_UPGRADE,
            HI_ACCEPT_ENCODING,
            HI_VIA,
            HI_AUTHORIZATION,
            HI_KEEPALIVE,
            HI_XFORWARD_FOR,
            HI_X_REAL_IP,
            HI_ACCEPT,
            HI_ACCEPT_LANGUAGE,
            HI_DEPTH,
            HI_DESTINATION,
            HI_OVERWRITE,
            HI_DATE,
            HI_COOKIE,
            HI_SIZE
        };

        struct ConstStringHash {
            const char *String;
            uint32_t Hash;
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

