#include "HTTP/HTTPError.h"

using namespace ngx::HTTP;

const char *HTTPError::ErrorCodeToError(int ErrorCode) {
    return "Unspecified error!";
}