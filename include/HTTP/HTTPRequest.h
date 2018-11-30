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
    Array<HTTPHeader> Headers;
    Array<BoundCursor> ArgumentList;
    HTTPRequestState State = HTTP_INIT_STATE;

    friend class HTTPParser;

public:
    HTTPRequest(Allocator *Allocator) : Headers(Allocator), ArgumentList(Allocator) {};
    /* HTTPConnection
    * CanReset
    * enum Method
    * Range URI
    * Array *Headers
    * Range Data
    *
    * */
};
