namespace ngx {
    namespace HTTP {

        struct HTTPHeader {
            BoundCursor Name;
            BoundCursor Value;
            bool Valid;

            HTTPHeader() : Name(), Value(), Valid(true) {}
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
            Array Headers;
            Array ArgumentList;
            HTTPRequestState State = HTTP_INIT_STATE;

            friend class HTTPParser;

        public:
            HTTPRequest(MemAllocator *Allocator) : Headers(Allocator, sizeof(HTTPHeader)),
                                                   ArgumentList(Allocator, sizeof(BoundCursor)) {};
            /* HTTPConnection
            * CanReset
            * enum Method
            * Range URI
            * Array *Headers
            * Range Data
            *
            * */
        };
    }
}