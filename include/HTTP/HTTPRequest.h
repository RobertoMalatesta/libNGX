struct HTTPHeaderIn {
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

    Array<HTTPHeader> XForwardFor;

    BoundCursor XRealIP;

    BoundCursor Accept;
    BoundCursor AcceptLanguage;

    BoundCursor User;
    BoundCursor Passwrod;

    Array<BoundCursor> Cookies;

    BoundCursor Server;

    uint32_t ContentLengthUInt32;
    uint32_t KeepAliveUInt32;

    unsigned ConnectionType:2;
    unsigned Chunked:1;

    Array<HTTPHeader> OtherHeaders;

    HTTPHeaderIn(Allocator *BackendAllocator): XForwardFor(BackendAllocator), Cookies(BackendAllocator), OtherHeaders(BackendAllocator){};

};


class HTTPRequest {
protected:

    unsigned ComplexURI:1;
    unsigned QuotedURI:1;
    unsigned PlusInURI:1;
    unsigned SpaceInURI:1;
    unsigned short Version;

    HTTPMethod Method;
    BoundCursor URI;
    BoundCursor URIExt;
    BoundCursor Port;
    BoundCursor HTTPProtocol;
    BoundCursor Argument;
    BoundCursor Content;
    BoundCursor Schema;
    BoundCursor Host;
    BoundCursor IP;

    HTTPHeaderIn HeaderIn;
    // HTTPResponse Response;

    HTTPRequestState State = HTTP_INIT_STATE;

    friend class HTTPParser;

public:
    HTTPRequest(Allocator *Allocator): HeaderIn(Allocator) {};
    /* HTTPConnection
    * CanReset
    * enum Method
    * Range URI
    * Array *Headers
    * Range Data
    *
    * */
};
