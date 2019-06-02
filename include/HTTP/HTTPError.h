#include "Core/Support/Error.h"

#ifndef NGX_HTTP_ERROR
#define NGX_HTTP_ERROR

namespace ngx {
namespace HTTP {

using namespace ngx::Core::Support;

class HTTPError : public Error {
private:
    static const char *ErrorCodeToError(int ErrorCode);

public:
    HTTPError(int ErrorCode, const char *Message = nullptr) : Error(ErrorCode, Message) {};

    const char *GetError() {
        return ErrorCodeToError(ErrorCode);
    }

    virtual void PrintError() {
        printf("HTTPError: %s, Message: %s\n", ErrorCodeToError(ErrorCode),
               Message == nullptr ? "null" : Message);
    }
};
} // HTTP
} // ngx
#endif