namespace ngx{
    namespace Http {

        enum HttpRequestState {
            HTTP_BAD_REQUEST_STATE = -1,
            HTTP_INIT_STATE,
            HTTP_PAESE_METHOD,
            HTTP_PARSE_REQUEST_LINE,
            };

        enum HttpRequestLineParseState {
            RL_Space_Before_URI = 0,
            RL_Schema,
            RL_SchemaSlash,
            RL_SchemaSlashSlash,
            RL_HostStart,
            RL_Host,
            RL_HostEnd,
            RL_HostIpLiterial,
            RL_Port,
            RL_HostHTTP09,
            RL_AfterSlashInURI,
            RL_CheckURI,
            RL_CheckURIHTTP09,
            RL_URI,
            RL_HTTP09,
            RL_HTTP_H,
            RL_HTTP_HT,
            RL_HTTP_HTT,
            RL_HTTP_HTTP,
            RL_FirstMajorDigit,
            RL_MajorDigit,
            RL_FirstMinorDigit,
            RL_MinorDigit,
            RL_SpaceAfterDigit,
            RL_AlmostDone
        };

        class HttpRequestContext {
        protected:
            unsigned int HttpMajor;
            unsigned int HttpMinor;
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
            HttpRequestState State = HTTP_INIT_STATE;

            HttpError ParseMethod(Buffer &B);
            HttpError ParseRequestLine(Buffer &B);
            HttpError ParseHeaders(Buffer &B);
        public:

            virtual void Reset();

            HttpError ProcessHttpRequest(Buffer &B);


        };
    }
}