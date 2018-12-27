struct HTTPHeaderOut {

    // HTTP Core Headers
    // HTTP Custom Headers
    Array<HTTPHeader> Headers;
    Array<HTTPHeader> Trailers;

    HTTPHeaderOut(Allocator *BackendAllocator): Headers(BackendAllocator), Trailers(BackendAllocator){};
};

class HTTPResponse: public CanReset {
protected:

    enum HTTPResponseState {
        HTTP_BAD_REQUEST_STATE = -1,
        HTTP_INIT_STATE,
        HTTP_PAESE_METHOD,
        HTTP_PARSE_REQUEST_LINE,
        HTTP_PARSE_HEADER,
        HTTP_HEADER_DONE,
    };

    // HTTPResponse Parse State
    HTTPResponseState State = HTTP_INIT_STATE;
    HTTPHeaderOut HeaderOut;

public:

    HTTPResponse(Allocator *Allocator): HeaderOut(Allocator) {
        Reset();
    };

    /** Read a request from buffer */
    HTTPError ReadResponse(Buffer &B);

    /** Write a request to buffer */
    HTTPError WriteResponse(Buffer &B);

    /** Reset state and release memory */
    virtual void Reset();
};
