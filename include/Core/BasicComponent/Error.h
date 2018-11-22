//===- Error.h - Implement Error classes specification  ------------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file declare Error classes.
//
//===-------------------------------------------------------------------------===//

class Error {
protected:
    int ErrorCode;
    const char *Message;
public:
    Error(int ErrorCode, const char *Message = nullptr) {
        this->ErrorCode = ErrorCode;
        this->Message = Message;
    };

    int GetCode() { return this->ErrorCode; }

    const char *CodeMessage() { return Message; }

    virtual void PrintError() { printf("BaseError: Unspecfied error!\n"); }
};

class RuntimeError : public Error {
private:
    static const char *ErrorCodeToError(int ErrorCode);

public:
    RuntimeError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

    const char *GetError() {
        return ErrorCodeToError(ErrorCode);
    }

    virtual void PrintError() {
        printf("RuntimeError: %s, Message: %s\n", ErrorCodeToError(ErrorCode),
               Message == nullptr ? "null" : Message);
    }
};

class SocketError : public Error {
private:
    static const char *ErrorCodeToError(int ErrorCode);

public:
    SocketError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

    const char *GetError() { return ErrorCodeToError(ErrorCode); }

    virtual void PrintError() {
        printf("SocketError: %s, Message: %s\n", ErrorCodeToError(ErrorCode),
               Message == nullptr ? "null" : Message);
    }
};

class EventError : public Error {
private:
    static const char *ErrorCodeToError(int ErrorCode);

public:
    EventError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

    const char *GetError() { return ErrorCodeToError(ErrorCode); }

    virtual void PrintError() {
        printf("EventError: %s, Message: %s\n", ErrorCodeToError(ErrorCode),
               Message == nullptr ? "null" : Message);
    }
};

