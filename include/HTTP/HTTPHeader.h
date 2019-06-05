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
    StringRef Header, Value;
public:
    HTTPHeader() : Header(), Value() {}
    HTTPError parse(MemoryBuffer &B, size_t &Off, bool Underscore);
    HTTPError write(BufferWriter &W);
    HTTPError Read(Buffer &B, bool AllowUnderscore);
    HTTPError Write(Buffer &B);
};
} //    HTTP
}   // ngx
#endif
