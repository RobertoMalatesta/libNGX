#include "HTTP/HTTPError.h"
#include "HTTP/HTTPHeader.h"
#include "HTTP/HTTPCoreHeader.h"
#ifndef NGX_HTTP_REQUEST
#define NGX_HTTP_REQUEST
namespace ngx {
namespace HTTP {
using namespace ngx::Core::Support;
class HTTPRequest {
protected:
    // HTTPRequest Parse State
    enum HTTPRequestState {
        HTTP_BAD_REQUEST = -1,
        HTTP_INIT,
        HTTP_PAESE_METHOD,
        HTTP_PARSE_REQUEST_LINE,
        HTTP_PARSE_HEADER,
        HTTP_HEADER_DONE,
    } State=HTTP_INIT;
    size_t HeadersOffset;
    void processCoreHeader(HTTPHeader &H, uint32_t hash);
    /** Parse HTTP Method from Buffer into HTTPRequest */
    HTTPError parseMethod(const StringRef &TopHalf);
    /** Parse HTTP Request Line from Buffer into HTTPRequest */
    HTTPError parseRequestline(const StringRef &TopHalf);
    /** Parse HTTP Header In through ParseHeader, and do some process */
    HTTPError parseHeaders(const StringRef &TopHalf);
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
    // HTTP Version
    unsigned short Version;
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
    // HTTP URI Flags
    struct {
        unsigned ComplexURI:1;
        unsigned QuotedURI:1;
        unsigned PlusInURI:1;
        unsigned SpaceInURI:1;
    };
    // Core header variable
    struct {
        uint32_t ContentLength;
        uint32_t Keepalive;
        unsigned ConnectionType:2;
        unsigned Chunked:1;
    } CoreHeaderVariable;
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
    // HTTP Custom Headers
    SmallVector<std::pair<uint32_t , HTTPHeader>, 16> HeaderMap;
    // HTTP Forward Headers
    SmallVector<StringRef, 8> XForwardFor;
    // Cookies extracted from Cookie Header
    SmallVector<StringRef, 8> Cookies;
    StringRef BodyTH, BodyBH;

    HTTPRequest()= default;
    HTTPError parseRequestURI(const StringRef &S);
    HTTPError parse(const StringRef &TopHalf, const StringRef &BottomHalf);
    HTTPError write(BufferWriter &W);
};
} // HTTP
} // ngx
#endif