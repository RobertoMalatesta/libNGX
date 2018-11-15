namespace ngx {
    namespace HTTP {

        struct HTTPHeader {
            HashBoundCursor Name;
            BoundCursor Value;
            bool Valid;

            HTTPHeader() : Name(), Value(), Valid(true) {}
        };

        class HTTPRequest {
        protected:
            unsigned int ComplexURI:1;
            unsigned int QuotedURI:1;
            unsigned int PlusInURI:1;
            unsigned int SpaceInURI:1;
            unsigned int Version;

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
            BoundCursor Request;
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