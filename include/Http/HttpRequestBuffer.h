namespace ngx::Http {

    enum HttpRequestStatus {
        HTTP_IDEL_STATE = 0,
        HTTP_PARSE_REQUEST_LINE,
        HTTP_VALIDATE_HOST,
        HTTP_PROCESS_REQUEST_URI,
    };

    class HttpRequestBuffer : Buffer {
        protected:
            uint32_t HttpVersion = 0;
            HttpRequestStatus Status = HTTP_IDEL_STATE;
            void HttpParseRequestLine();
            void HttpProcessRequestHeader();
            void HttpValidateHost();
            void HttpProcessRequestURI();
        public:
            HttpRequestBuffer(BufferMemoryBlockRecycler *R, size_t BlockSize);
            ~HttpRequestBuffer();

            RuntimeError WriteDataToBuffer(HttpConnection *C);

            void ResetState();
            bool IsInProcess();
            void ProcessHttpRequest(void *Argument, ThreadPool *TPool);
    };
}