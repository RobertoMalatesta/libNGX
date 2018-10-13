namespace ngx::Http {

    enum HttpMethod {
        GET,
        PUT,
        POST,
        COPY,
        MOVE,
        LOCK,
        HEAD,
        MKCOL,
        PATCH,
        TRACE,
        DELETE,
        UNLOCK,
        OPTIONS,
        PROPFIND,
        PROPPATCH,
    };

    enum HttpRequestStatus {
        HTTP_BAD_REQUEST_STATE = -1,
        HTTP_IDEL_STATE,
        HTTP_PAESE_METHOD,
        HTTP_PARSE_SPACE_BEFORE_URI,
        HTTP_PARSE_SCHEMA,
        HTTP_PARSE_REQUEST_LINE,
        HTTP_VALIDATE_HOST,
        HTTP_PROCESS_REQUEST_URI,
    };

    class HttpRequestBuffer : public Buffer {
        protected:
            uint32_t HttpVersion = 0;
            HttpRequestStatus Status = HTTP_IDEL_STATE;
        public:
            HttpRequestBuffer(BufferMemoryBlockRecycler *R, size_t BlockSize);
            ~HttpRequestBuffer();

            void ProcessHttpRequest();
            RuntimeError WriteDataToBuffer(HttpConnection *C);
            virtual void Reset() {};
    };
}