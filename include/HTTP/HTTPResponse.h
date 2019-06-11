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
    size_t HeadersOffset;
    StringRef StatusCodeStr;
    StringRef StatusText;
    StringRef StatusLine;
    StringRef BodyTH, BodyBH;
    SmallVector<std::pair<uint32_t, HTTPHeader>> HeaderMap;
    SmallVector<std::pair<uint32_t, HTTPHeader>> Trailers;
    /** Parse HTTP Response Line from Buffer to HTTPResponse */
    void processCoreHeader(HTTPHeader &H, uint32_t hash);
    HTTPError parseResponseline(const StringRef &TopHalf);
    HTTPError parseHeaders(const StringRef &TopHalf);
public:

    HTTPResponse() = default;
    int getVersion() const { return Version; }
    int getStatusCode() const { return StatusCode; }
    const StringRef &getStatusCodeString() const { return StatusCodeStr; }
    const StringRef &getStatusText() const { return StatusText; }
    const StringRef &getStatusLine() const { return StatusLine; }
    const SmallVector<std::pair<uint32_t, HTTPHeader>> &getHeaders() const { return HeaderMap; }
    const SmallVector<std::pair<uint32_t, HTTPHeader>> &getTrailers() const { return Trailers; }
    HTTPError parse(const StringRef &TopHalf, const StringRef &BottomHalf);
    HTTPError write(BufferWriter &W);
};
}
};
#endif