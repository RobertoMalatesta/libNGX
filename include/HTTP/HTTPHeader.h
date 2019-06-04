#ifndef NGX_HTTP_HEADER
#define NGX_HTTP_HEADER

#include "Core/Support/BoundCursor.h"
#include "Core/Support/Buffer.h"
#include "HTTP/common_header.h"
#include "HTTP/HTTPCoreHeader.h"

namespace ngx {
namespace HTTP {
using namespace ngx::Core::Support;
typedef HTTPError (HTTPHeaderProcess)(HTTPCoreHeader &, HTTPRequest &, HTTPHeader &H);
struct HTTPHeader {
    BoundCursor Name;
    BoundCursor Value;
    bool Valid;

    HTTPHeader() : Name(), Value(), Valid(true) {}

    HTTPError Read(Buffer &B, bool AllowUnderscore);

    HTTPError Write(Buffer &B);
};
} //    HTTP
}   // ngx
#endif
