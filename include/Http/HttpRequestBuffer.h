namespace ngx::Http {

    const u_char CR = '\r';
    const u_char LF = '\n';

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

    enum HttpRequestLineParseState{
        RL_Start = 0,
        RL_Method,
        RL_Space_Before_Uri,
        RL_Schema,
        RL_SchemaSlash,
        RL_SchemaSlashSlash,
        RL_HostStart,
        RL_Host,
        RL_HostEnd,
        RL_HostIpLiterial,
        RL_Port,
        RL_HostHttp09,
        RL_AfterSlashInUri,
        RL_CheckUri,
        RL_CheckUriHttp09,
        RL_Uri,
        RL_Http09,
        RL_Http_H,
        RL_Http_HT,
        RL_Http_HTT,
        RL_Http_HTTP,
        RL_FirstMajorDigit,
        RL_MajorDigit,
        RL_FirstMinorDigit,
        RL_MinorDigit,
        RL_SpaceAfterDigit,
        RL_AlmostDone
    } ;

    class HttpRequestBuffer : public Buffer {
        protected:
            HttpVersion Version;
            HttpMethod Method;
            HttpRequestState State = HTTP_INIT_STATE;
            HttpRequestLineParseState RequestLineState;
        public:
            HttpRequestBuffer(BufferMemoryBlockRecycler *R, size_t BlockSize);
            ~HttpRequestBuffer() = default;
            HttpError ProcessHttpRequest();
            HttpError ParseRequestLine();
            RuntimeError WriteDataToBuffer(HttpConnection *C);
            virtual void Reset();
    };
}