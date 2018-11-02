#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

const char *HTTPError::ErrorCodeToString(int ErrorCode) {
    return "Unspecified error!";
}