#include "Core/ADT/SmallVector.h"
#include "Core/Support/Support.h"
#ifndef NGX_HTTP_RESPONSE
#define NGX_HTTP_RESPONSE
namespace ngx {
namespace HTTP {
using namespace ngx::Core::Support;
class HTTPResponse {
protected:
    enum HTTPResponseState {
        HTTP_BAD_RESPONSE = -1,
        HTTP_INIT,
        HTTP_PARSE_RESPONSE_LINE,
        HTTP_PARSE_HEADERS,
        HTTP_HEADER_DONE,
    } State = HTTP_INIT;
    int Version;
    int StatusCode;
    StringRef StatueCodeStr;
    StringRef StatusText;
    SmallVector<std::pair<uint32_t, HTTPHeader>> HeaderMap;
    SmallVector<std::pair<uint32_t, HTTPHeader>> Trailers;
    /** Parse HTTP Response Line from Buffer to HTTPResponse */
    HTTPError parseResponseline(WritableMemoryBuffer &B, size_t Size);

public:
    HTTPResponse() = default;
    HTTPError parse(WritableMemoryBuffer &TopHalf, size_t THSize,
                    WritableMemoryBuffer &BottomHalf, size_t BHSize);
    HTTPError write(WritableMemoryBuffer &B, BufferWriter &W);
};
}
};
#endif