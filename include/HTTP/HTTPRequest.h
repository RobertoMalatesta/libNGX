namespace ngx{
    namespace HTTP {

        class HTTPRequest {
        protected:
            unsigned int HttpMajor;
            unsigned int HTTPMinor;
            unsigned int ComplexURI:1;
            unsigned int QuotedURI:1;
            unsigned int PlusInURI:1;
            unsigned int SpaceInURI:1;

            SpinLock Lock;
            unsigned int Version;
            HttpMethod Method;
            Range URI;
            Range URIExt;
            Range Schema;
            Range Host;
            Range IP;
            Range Port;
            Range HTTPProtocol;
            Range Arguments;
            Range Request;
            HTTPRequestState State = HTTP_INIT_STATE;
        friend class HTTPParser;
            /* HTTPConnection
            * Resetable
            * enum Method
            * Range URI
            * Array *Headers
            * Range Data
            *
            * */

        };
    }}