

class HTTPParser {
protected:
    static HTTPError ParseMethod(Buffer &B, HTTPRequest &R);

    static HTTPError ParseRequestLine(Buffer &B, HTTPRequest &R);

    static HTTPError
    ParseHeaders(Buffer &B, HTTPRequest &R, bool AllowUnderScore = DEFAULT_HTTP_ALLOW_UNDERSCORE);

    static HTTPError ProcessHeaders(HTTPRequest &R);

    static HTTPError ValidateURI(HTTPRequest &R);

    static HTTPError ValidateComplexURI(HTTPRequest &R);

    static HTTPError ParseResponseLine(Buffer &B, HTTPRequest &R);

    static HTTPError ReadDataChunk(Buffer &B, HTTPRequest &R);

public:
    static HTTPError ParseHTTPRequest(Buffer &B, HTTPRequest &R);
};
