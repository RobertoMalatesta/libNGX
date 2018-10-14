namespace ngx::Core {

    class Error {
        protected:
            int ErrorCode;
            const char *Message;
        public:
            Error(int ErrorCode, const char *Message = nullptr) {
                this->ErrorCode = ErrorCode;
                this->Message = Message;
            };
            int GetErrorCode() { return this->ErrorCode; }
            const char *GetErrorMessage() { return Message; }
            virtual void PrintError() { printf("BaseError: Unspecfied error!\n"); }
    };

    class RuntimeError : public  Error {
        private:
            static const char *ErrorCodeToString(int ErrorCode);
        public:
            RuntimeError(int ErrorCode, const char *Message = nullptr):Error(ErrorCode, Message) {};
            const char *GetErrorString() {
                return ErrorCodeToString(ErrorCode);
            }
            virtual void PrintError() {
                printf("RuntimeError: %s, Message: %s\n", ErrorCodeToString(ErrorCode), Message == nullptr? "null": Message);
            }
    };

    class SocketError : public  Error {
        private:
            static const char *ErrorCodeToString(int ErrorCode);
        public:
            SocketError(int ErrorCode, const char *Message = nullptr):Error(ErrorCode, Message) {};
            const char *GetErrorString() { return ErrorCodeToString(ErrorCode); }
        virtual void PrintError() {
                printf("SocketError: %s, Message: %s\n", ErrorCodeToString(ErrorCode), Message == nullptr? "null": Message);
            }
    };

    class EventError : public  Error {
        private:
            static const char *ErrorCodeToString(int ErrorCode);
        public:
            EventError(int ErrorCode, const char *Message = nullptr):Error(ErrorCode, Message) {};
            virtual void PrintError() {
                printf("EventError: %s, Message: %s\n", ErrorCodeToString(ErrorCode), Message == nullptr? "null": Message);
            }
    };
};