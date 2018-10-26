namespace ngx{
    namespace Http {

        enum HttpRequestState {
            HTTP_BAD_REQUEST_STATE = -1,
            HTTP_INIT_STATE,
            HTTP_PAESE_METHOD,
            HTTP_PARSE_SPACE_BEFORE_URI,
            HTTP_PARSE_SCHEMA,
            HTTP_PARSE_REQUEST_LINE,
            HTTP_VALIDATE_HOST,
            HTTP_PROCESS_REQUEST_URI,
            };

        enum HttpRequestLineParseState {
            RL_Start = 0,
            RL_Method,
            RL_Space_Before_URI,
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
            unsigned int HttpMinor;
            unsigned int ComplexURI;
            unsigned int QuotedURI;
            unsigned int PlusInURI;

            SpinLock Lock;
            HttpVersion Version;
            HttpMethod Method;
            BufferRange URI;
            BufferRange URIExt;
            BufferRange Schema;
            BufferRange Host;
            BufferRange IP;
            BufferRange Port;
            BufferRange HTTPProtocol;
            BufferRange Arguments;
            HttpRequestState State = HTTP_INIT_STATE;
            HttpRequestLineParseState RequestLineState;
        public:

            virtual void Reset();

            HttpError ProcessHttpRequest(Buffer &B);

            HttpError ParseRequestLine(Buffer &B);

        };
    }
}