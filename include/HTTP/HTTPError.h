namespace ngx{
    namespace HTTP {

        class HTTPError : public Error {
        private:
            static const char *ErrorCodeToError(int ErrorCode);

        public:
            HTTPError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

            const char *GetError() {
                return ErrorCodeToError(ErrorCode);
            }

            virtual void PrintError() {
                printf("HTTPError: %s, Message: %s\n", ErrorCodeToError(ErrorCode), Message == nullptr ? "null" : Message);
            }
        };
    }
}