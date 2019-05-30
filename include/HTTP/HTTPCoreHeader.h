#ifndef NGX_HTTP_CORE_HEADER
#define NGX_HTTP_CORE_HEADER

#include "Core/Core.h"
#include "HTTP/common_header.h"
#include "HTTP/HTTPHeader.h"

namespace ngx {
    namespace HTTP {
        using namespace ngx::Core::Support;

        struct HTTPCoreHeader : public DictionaryItem {
            HTTPCoreHeaderIn Type;
            HTTPHeaderProcess *Setter = nullptr;
            virtual uint32_t HashFn() const;
            HTTPCoreHeader(const char *Key, HTTPCoreHeaderIn HeaderInEnum, HTTPHeaderProcess *HeaderProcess);
            inline const char *GetKey() const { return Key; }
            inline HTTPCoreHeaderIn GetType() const { return Type; }
            HTTPError Process(HTTPRequest &Request, HTTPHeader &Header);
        };
    } // HTTP
} // ngx
#endif