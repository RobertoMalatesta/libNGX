#ifndef NGX_HTTP_HEADER
#define NGX_HTTP_HEADER

#include "Core/Core.h"
#include "HTTP/common_header.h"

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
