#include "Core/Core.h"

using namespace ngx::Core;

const char *RuntimeError::ErrorMessage(int ErrorCode) {
    switch (ErrorCode) {
        case 0:
            return "Success!";
        case EINVAL:
            return "Invalid Access!";
        default:
            return "Unspecified error!";
    }
}

const char *SocketError::ErrorMessage(int ErrorCode) {
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