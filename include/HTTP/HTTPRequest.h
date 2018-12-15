struct HTTPHeaderIn {

    // HTTP Core Headers
    BoundCursor Host;
    BoundCursor Connection;
    BoundCursor IfModufiedSince;
    BoundCursor IfUnModifiedSince;
    BoundCursor IfMatch;
    BoundCursor IfNonMatch;
    BoundCursor UserAgent;
    BoundCursor Referer;
    BoundCursor ContentLength;
    BoundCursor ContentRange;
    BoundCursor ContentType;
    BoundCursor Range;
    BoundCursor IfRange;
    BoundCursor TransferEncoding;
    BoundCursor TE;
    BoundCursor Except;
    BoundCursor Upgrade;
    BoundCursor AcceptEncoding;
    BoundCursor Via;
    BoundCursor Authorization;
    BoundCursor KeepAlive;
    BoundCursor XRealIP;
    BoundCursor Accept;
    BoundCursor AcceptLanguage;
    BoundCursor User;
    BoundCursor Passwrod;
    BoundCursor Cookie;
    BoundCursor Server;

    // HTTP Custom Headers
    Array<HTTPHeader> Headers;

    // HTTP Forward Headers
    Array<HTTPHeader> XForwardFor;

    // Cookies extracted from Cookie Header
    Array<BoundCursor> Cookies;

    // Parsed HTTP Core Parameters
    struct {
        uint32_t ContentLengthSize;
        uint32_t KeepAliveTime;
        unsigned ConnectionType:2;
        unsigned Chunked:1;
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

    HTTPHeaderIn(Allocator *BackendAllocator): XForwardFor(BackendAllocator), Cookies(BackendAllocator), Headers(BackendAllocator){};
};

class HTTPRequest {
protected:

    // HTTP Method GET, POST, PUT, DELETE, ...
    HTTPMethod Method;

    // URI http URI index
    BoundCursor URI;

    // Postfix of HTTP URI Entity
    BoundCursor URIExt;

    // HTTP Protocol Version eg: HTTP/1.1
    BoundCursor HTTPProtocol;

    // Pointer to request line argument
    BoundCursor Argument;

    // Schema Proxy Schema?
    BoundCursor Schema;

    // Proxy Host
    BoundCursor Host;

    // Proxy IP
    BoundCursor IP;

    // Proxy Port
    BoundCursor Port;

    // HTTPRequest Headers
    HTTPHeaderIn HeaderIn;

    // HTTPRequest Parse State
    HTTPRequestState State = HTTP_INIT_STATE;

    // HTTP URI Flags
    struct {
        unsigned ComplexURI:1;
        unsigned QuotedURI:1;
        unsigned PlusInURI:1;
        unsigned SpaceInURI:1;
    };

    // HTTP Version
    unsigned short Version;

    friend class HTTPParser;

public:
    HTTPRequest(Allocator *Allocator): HeaderIn(Allocator) {};
};
