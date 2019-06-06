#ifndef NGX_HTTP_HEADER
#define NGX_HTTP_HEADER

#include "Core/Support/MemoryBuffer.h"
#include "Core/Support/Buffer.h"
#include "HTTP/common_header.h"
#include "HTTP/HTTPCoreHeader.h"

namespace ngx {
namespace HTTP {
using namespace ngx::Core::Support;
//typedef HTTPError (HTTPHeaderProcess)(HTTPCoreHeader &, HTTPRequest &, HTTPHeader &H);
class HTTPHeader {
    static StringRef &replaceCoreHeader(StringRef &origin);
    StringRef Header, Value;
public:
    enum CoreHeaderEnum {
        HI_HOST = 0,
        HI_CONNECTION,
        HI_IF_MODIFY_SINCE,
        HI_IF_UNMODIFY_SINCE,
        HI_IF_MATCH,
        HI_IF_NON_MATCH,
        HI_USERAGENT,
        HI_REFERENCE,
        HI_CONTENT_LENGTH,
        HI_CONTENT_RANGE,
        HI_CONTENT_TYPE,
        HI_RANGE,
        HI_IF_RANGE,
        HI_TRANSFER_ENCODING,
        HI_TE,
        HI_EXPECT,
        HI_UPGRADE,
        HI_ACCEPT_ENCODING,
        HI_VIA,
        HI_AUTHORIZATION,
        HI_KEEPALIVE,
        HI_X_FORWARD_FOR,
        HI_X_REAL_IP,
        HI_ACCEPT,
        HI_ACCEPT_LANGUAGE,
        HI_DEPTH,
        HI_DESTINATION,
        HI_OVERWRITE,
        HI_DATE,
        HI_COOKIE,
    };
    static SmallVector<std::pair<uint32_t, StringRef>, 64> CoreHeaderMap;
    static const StringRef ConstCoreHeader[];
    HTTPHeader() : Header(), Value() {}
    const StringRef &getHeader() const { return Header; }
    const StringRef &getValue() const { return Value; }
    HTTPError parse(MemoryBuffer &B, size_t &Off, bool Underscore);
    size_t write(BufferWriter &W);
};
} // HTTP
} // ngx
#endif
