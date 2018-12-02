class HTTPParser {
protected:

    const HTTPCoreHeader HeaderInProcesses[31] = {
            {"Host", HI_HOST, nullptr},
            {"Connection", HI_CONNECTION, nullptr},
            {"If-Modified-Since", HI_IF_MODIFY_SINCE, nullptr},
            {"If-Unmodified-Since", HI_IF_UNMODIFY_SINCE, nullptr},
            {"If-Match", HI_IF_MATCH, nullptr},
            {"If-None-Match", HI_IF_NON_MATCH, nullptr},
            {"UserAgent", HI_USERAGENT, nullptr},
            {"Referer", HI_REFERENCE, nullptr},
            {"Content-Length", HI_CONTENT_LENGTH, nullptr},
            {"Content-Range", HI_CONTENT_RANGE, nullptr},
            {"Content-Type", HI_CONTENT_TYPE, nullptr},
            {"Range", HI_RANGE, nullptr},
            {"If-Range", HI_IF_RANGE, nullptr},
            {"Transfer-Encoding", HI_TRANSFER_ENCODING, nullptr},
            {"TE", HI_TE, nullptr},
            {"Epect", HI_EXPECT, nullptr},
            {"Upgrade", HI_UPGRADE, nullptr},
            {"Accept-Encoding", HI_ACCEPT_ENCODING, nullptr},
            {"Via", HI_VIA, nullptr},
            {"Authorization", HI_AUTHORIZATION, nullptr},
            {"Keep-Alive", HI_KEEPALIVE, nullptr},
            {"X-Forward-For", HI_XFORWARD_FOR, nullptr},
            {"X-Real-IP", HI_X_REAL_IP, nullptr},
            {"Accept", HI_ACCEPT, nullptr},
            {"Accept-Language", HI_ACCEPT_LANGUAGE, nullptr},
            {"Depth", HI_DEPTH, nullptr},
            {"Destinaion", HI_DESTINATION, nullptr},
            {"Overwrite", HI_OVERWRITE, nullptr},
            {"Date", HI_DATE, nullptr},
            {"Cookie", HI_COOKIE, nullptr},
            {"Size", HI_SIZE, nullptr},
    };

    Dictionary HeaderProcess;

    static HTTPError ParseMethod(Buffer &B, HTTPRequest &R);

    static HTTPError ParseRequestLine(Buffer &B, HTTPRequest &R);

    static HTTPError ParseHeaders(Buffer &B, HTTPRequest &R, bool AllowUnderScore = ALLOW_UNDERSCORE);

    static HTTPError BasicHeaderProcess(HTTPCoreHeader &CH, HTTPRequest &R, HTTPHeader&H);

    static HTTPError ValidateURI(HTTPRequest &R);

    static HTTPError ValidateComplexURI(HTTPRequest &R);

    static HTTPError ParseResponseLine(Buffer &B, HTTPRequest &R);

    static HTTPError ReadDataChunk(Buffer &B, HTTPRequest &R);
public:

    HTTPParser();

    HTTPError ParseHTTPRequest(Buffer &B, HTTPRequest &R);

    HTTPError HeaderInProcess(HTTPRequest &R, HTTPHeader &H);
};

