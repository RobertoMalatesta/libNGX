namespace ngx{
    namespace HTTP {

        class HTTPError : public Error {
        private:
            static const char *ErrorCodeToString(int ErrorCode);

        public:
            HTTPError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

            const char *GetErrorString() {
                return ErrorCodeToString(ErrorCode);
            }

            virtual void PrintError() {
                printf("HTTPError: %s, Message: %s\n", ErrorCodeToString(ErrorCode), Message == nullptr ? "null" : Message);
            }
        };
    }
}