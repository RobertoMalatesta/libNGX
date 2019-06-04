#include "HTTP/HTTPError.h"

using namespace ngx::HTTP;

const char *HTTPError::ErrorCodeToError(int ErrorCode) {
    switch (ErrorCode) {
        case 0:
            return "Succeed";
        default:
            return "Unspecified error!";
    }
}