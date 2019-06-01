#include <cerrno>
#include "Core/Support/Error.h"

using namespace std;
using namespace ngx::Core::Support;

const char *RuntimeError::ErrorCodeToError(int ErrorCode) {
    switch (ErrorCode) {
        case 0:
            return "Success!";
        case EINVAL:
            return "Invalid Access!";
        default:
            return "Unspecified error!";
    }
}

const char *SocketError::ErrorCodeToError(int ErrorCode) {
    switch (ErrorCode) {
        case 0:
            return "Success!";
        case EACCES:
            return "Access denied!";
        case EADDRINUSE:
            return "Address already in use!";
        case EINVAL:
            return "Invalid socket address!";
        default:
            return "Unspecified error!";
    }
}

const char *EventError::ErrorCodeToError(int ErrorCode) {
    switch (ErrorCode) {
        case 0:
            return "Success!";
        default:
            return "Unspecified error!";
    }
}