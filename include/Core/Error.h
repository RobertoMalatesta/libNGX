namespace ngx::Core {

    class Error {
        public:
            Error() = default;
            void PrintError() { printf("BaseError: Unspecfied error!\n"); }
    };

    class SocketError : public  Error {
        private:
            int ErrorCode;
            static const char *ErrorMessage(int ErrorCode);
        public:
            SocketError(int ErrorCode):Error() {
                this->ErrorCode = ErrorCode;
            };
            const char *GetErrorMessage() {return ErrorMessage(ErrorCode);}
            void PrintError() {printf("SocketError: %s\n", ErrorMessage(ErrorCode));}
            int GetErrorCode() { return this->ErrorCode; }
    };

};