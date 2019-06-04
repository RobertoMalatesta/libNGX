#ifndef NGX_HTTP_REQUEST
#define NGX_HTTP_REQUEST

#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPCoreHeader.h"

namespace ngx {
namespace HTTP {
using namespace ngx::Core::Support;

struct HTTPHeaderIn {
    // HTTP Core HeadersIn
    struct {
        uint32_t ContentLengthSize;
        uint32_t KeepAliveTime;
        unsigned ConnectionType:2;
        unsigned Chunked:1;
    };
    HTTPHeaderIn() {};
};

class HTTPRequest {
public:
    enum Method {
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
        HTTP_BAD_REQUEST = -1,
        HTTP_INIT,
        HTTP_PAESE_METHOD,
        HTTP_PARSE_REQUEST_LINE,
        HTTP_PARSE_HEADER,
        HTTP_HEADER_DONE,
    };
    // HTTP Method GET, POST, PUT, DELETE, ...
    Method Method;
    StringRef MethodStr;
    StringRef RequestLine;
    // URI http URI index
    StringRef URI;
    // HTTP Protocol Version eg: HTTP/1.1
    StringRef Protocol;
    // Pointer to request line argument
    StringRef Argument;
    // Schema Proxy Schema?
    StringRef Scheme;
    // Proxy Host
    StringRef Host;
    // Proxy IP
    StringRef IP;
    // Proxy Port
    StringRef Port;
    // HTTPRequest Headers
    HTTPHeaderIn HeaderIn;
    // HTTPRequest Parse State
    HTTPRequestState State = HTTP_INIT;
    // HTTP Version
    unsigned short Version;
    // HTTP URI Flags
    struct {
        unsigned ComplexURI:1;
        unsigned QuotedURI:1;
        unsigned PlusInURI:1;
        unsigned SpaceInURI:1;
    };
    // Browser info extracted from UserAgent
    struct {
        unsigned MSIE:1;
        unsigned MSIE6:1;
        unsigned Opera:1;
        unsigned Gecko:1;
        unsigned Chrome:1;
        unsigned Safari:1;
        unsigned Konqueror:1;
    };
    struct {
        StringRef Host;
        StringRef Connection;
        StringRef IfModufiedSince;
        StringRef IfUnModifiedSince;
        StringRef IfMatch;
        StringRef IfNonMatch;
        StringRef UserAgent;
        StringRef Referer;
        StringRef ContentLength;
        StringRef ContentRange;
        StringRef ContentType;
        StringRef Range;
        StringRef IfRange;
        StringRef TransferEncoding;
        StringRef TE;
        StringRef Except;
        StringRef Upgrade;
        StringRef AcceptEncoding;
        StringRef Via;
        StringRef Authorization;
        StringRef KeepAlive;
        StringRef XRealIP;
        StringRef Accept;
        StringRef AcceptLanguage;
        StringRef User;
        StringRef Passwrod;
        StringRef Cookie;
        StringRef Server;
    } CoreHeader;
    // HTTP Custom Headers
    SmallVector<HTTPHeader, 16> Headers;
    // HTTP Forward Headers
    SmallVector<StringRef, 16> XForwardFor;
    // Cookies extracted from Cookie Header
    SmallVector<StringRef, 16> Cookies;
    /** Parse HTTP Method from Buffer into HTTPRequest */
    HTTPError parseMethod(WritableMemoryBuffer &B);
    /** Parse HTTP Request Line from Buffer into HTTPRequest */
    HTTPError parseRequestLine(WritableMemoryBuffer &B);
    /** Parse HTTP Header In through ParseHeader, and do some process */
    HTTPError parseRequestHeaders(WritableMemoryBuffer &B, bool AllowUnderScore = ALLOW_UNDERSCORE);
    /** Identify Request URI and set related flags */
    HTTPError parseRequestURI(StringRef &S);
    /** Fill HTTP Core Header into HTTP Request */
    static HTTPError HeaderInFillVariable(HTTPCoreHeader &C, HTTPRequest &R, HTTPHeader &H);
    static HTTPCoreHeader HeaderInProcesses[];
public:
    HTTPRequest()= default;
    /** Read a request from buffer */
//    HTTPError Read(Buffer &B);
    /** Write a request to buffer */
//    HTTPError Write(Buffer &B);
    /** Reset state and release memory */
//    virtual void Reset();
};

} // HTTP
} // ngx
#endif