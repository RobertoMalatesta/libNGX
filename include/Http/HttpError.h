namespace ngx::Http {

    class HttpError : public Error {
    private:
        static const char *ErrorCodeToString(int ErrorCode);

    public:
        HttpError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

        const char *GetErrorString() {
            return ErrorCodeToString(ErrorCode);
        }

        virtual void PrintError() {
            printf("HttpError: %s, Message: %s\n", ErrorCodeToString(ErrorCode), Message == nullptr ? "null" : Message);
        }
    };
}