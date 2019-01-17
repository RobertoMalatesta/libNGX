#include "Core/Core.h"
#include "HTTP/config_header.h"

namespace ngx {
    namespace HTTP {

        using namespace ngx::Core;
        using namespace ngx::HTTP::DefaultConfig;

        const u_char LF = '\n';
        const u_char CR = '\r';
        const u_char CRLF[] = {CR, LF, '\0'};

        const uint32_t usual[] = {
                0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */
                            /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
                0x7fff37d6, /* 0111 1111 1111 1111  0011 0111 1101 0110 */
                            /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
                0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
                            /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
                0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
                0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
                0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
                0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
                0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        };

        const u_char lower_case[] =
                "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                "\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0" "0123456789\0\0\0\0\0\0"
                "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
                "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
                "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
                "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

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

        class HTTPServer;

        class HTTPConnection;

        class HTTPConnectionCollector;

        class HTTPConnectionBuilder;

        class HTTPRequest;

        class HTTPHeader;

        class HTTPCoreHeader;

        struct HTTPPMU;

#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPCoreHeader.h"
#include "HTTP/HTTPRequest.h"
#include "HTTP/HTTPResponse.h"
#include "HTTP/HTTPListening.h"
#include "HTTP/HTTPConnection.h"
#include "HTTP/HTTPConnectionBuilder.h"
#include "HTTP/HTTPServer.h"
#include "HTTP/Mux/Mux.h"

    }
}
