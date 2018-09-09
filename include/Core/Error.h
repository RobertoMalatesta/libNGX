namespace ngx::Core {

    class Error {
        public:
            Error() = default;
            void PrintError() { printf("BaseError: Unspecfied error!\n"); }
    };

    class RuntimeError : public  Error {
    private:
        int ErrorCode;
        static const char *ErrorMessage(int ErrorCode);
    public:
        RuntimeError(int ErrorCode):Error() {
            this->ErrorCode = ErrorCode;
        };
        const char *GetErrorMessage() {return ErrorMessage(ErrorCode);}
        void PrintError() {printf("RuntimeError: %s\n", ErrorMessage(ErrorCode));}
        int GetErrorCode() { return this->ErrorCode; }
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

    class EventError : public  Error {
        private:
            int ErrorCode;
            static const char *ErrorMessage(int ErrorCode);
        public:
            EventError(int ErrorCode):Error() {
                this->ErrorCode = ErrorCode;
            };
            const char *GetErrorMessage() {return ErrorMessage(ErrorCode);}
            void PrintError() {printf("EventError: %s\n", ErrorMessage(ErrorCode));}
            int GetErrorCode() { return this->ErrorCode; }
    };

};