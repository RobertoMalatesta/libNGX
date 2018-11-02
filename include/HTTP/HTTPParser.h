namespace ngx{
    namespace HTTP {


        class HTTPParser {
        protected:
            static HTTPError ParseMethod(Buffer &B, HTTPRequest &R);
            static HTTPError ParseRequestLine(Buffer &B, HTTPRequest &R);
            static HTTPError ParseHeaders(Buffer &B, HTTPRequest &R);
        public:
            static HTTPError ParseHTTPRequest(Buffer &B, HTTPRequest &R);
        };
    }
}