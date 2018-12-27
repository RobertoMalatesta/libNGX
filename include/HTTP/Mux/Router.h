class Router {
    /** Route filter a http request and return target service name */
    virtual BoundCursor Route(HTTPConnection &C, HTTPRequest &Request) = 0;
};