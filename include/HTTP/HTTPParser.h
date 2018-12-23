class HTTPParser {
protected:
// mark public for test
public:

    /** Parse HTTP Method from Buffer into HTTPRequest */
    static HTTPError ParseMethod(Buffer &B, HTTPRequest &R);

    /** Parse HTTP Request Line from Buffer into HTTPRequest */
    static HTTPError ParseRequestLine(Buffer &B, HTTPRequest &R);

    /** Parse HTTP Header from Buffer into HTTPHeader */
    static HTTPError ParseHeader(Buffer &B, HTTPHeader &Header, bool AllowUnderScore = ALLOW_UNDERSCORE);

    /** Parse HTTP Header In through ParseHeader, and do some process */
    static HTTPError ParseRequestHeaders(Buffer &B, HTTPRequest &R, bool AllowUnderScore = ALLOW_UNDERSCORE);

    /** Fill HTTP Core Header into HTTP Request */
    static HTTPError HeaderInFillVariable(HTTPCoreHeader &C, HTTPRequest &R, HTTPHeader &H);

    /** Write HTTP Request to Buffer */
//    HTTPError WriteRequest(HTTPRequest &R, Buffer &B);

    /** Parse HTTP Response Line from Buffer into HTTPResponse */
//    static HTTPError ParseResponseLine(Buffer &B, HTTPResponse &R);

    /** Parse HTTP Header Out through ParseHeader, and do some process */
//    HTTPError ParseResponseHeaders(Buffer &B, HTTPResponse &R, bool AllowUnderScore = ALLOW_UNDERSCORE);

    /** Write HTTP Response to Buffer */
//    HTTPError WriteResponse(HTTPResponse &R, Buffer &B);

    /** Judge if a request`s URI is valid */
    static HTTPError ValidateURI(HTTPRequest &R);

    /** Judge if a request`s complex URI is valid and set some flag when necessary */
    static HTTPError ValidateComplexURI(HTTPRequest &R);

    /** Read Data Chunk from buffer into HTTPRequest */
    static HTTPError ReadDataChunk(Buffer &B, HTTPRequest &R);

public:

    HTTPError ParseHTTPRequest(Buffer &B, HTTPRequest &R);
};

