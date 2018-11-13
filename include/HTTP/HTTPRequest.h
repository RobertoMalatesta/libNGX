namespace ngx {
    namespace HTTP {

        struct HTTPHeader {
            HashRange Name;
            Range Value;
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
            Range URI;
            BoundCursor URIExt;
            BoundCursor Port;
            BoundCursor HTTPProtocol;
            BoundCursor Argument;
            BoundCursor Content;
            Range Schema;
            Range Host;
            Range IP;
            Range Request;
            Array Headers;
            Array ArgumentList;
            HTTPRequestState State = HTTP_INIT_STATE;

            friend class HTTPParser;

        public:
            HTTPRequest(MemAllocator *Allocator) : Headers(Allocator, sizeof(HTTPHeader)),
                                                   ArgumentList(Allocator, sizeof(Range)) {};
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