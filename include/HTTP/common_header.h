#ifndef LIBNGX_COMMON_HEADER_H
#define LIBNGX_COMMON_HEADER_H
#include "common_header.h"
#include "Core/Support/MemoryBuffer.h"
#include "HTTP/config_header.h"
namespace ngx {
namespace HTTP {
using namespace Config;

const u_char LF = '\n';
const u_char CR = '\r';
const u_char CRLF[] = {CR, LF, '\0'};
const uint32_t usual[] = {
        0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */
        /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x7fff37d6, /* 0111 1111 1111 1111  0011 0111 1101 0110 */
        /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

enum HTTPCoreHeaderIn {
    HI_COMMON = -1,
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

using namespace ngx::Core::Support;
StringRef ConstCoreHeader[] = {
#define ITEM(x) {(Byte *)(x), reinterpret_cast<size_t>(sizeof(x)-1)}
       ITEM("Host"),
       ITEM("Connection"),
       ITEM("If-Modified-Since"),
       ITEM("If-Unmodified-Since"),
       ITEM("If-Match"),
       ITEM("If-None-Match"),
       ITEM("UserAgent"),
       ITEM("Referer"),
       ITEM("Content-Length"),
       ITEM("Content-Range"),
       ITEM("Content-Type"),
       ITEM("Range"),
       ITEM("If-Range"),
       ITEM("Transfer-Encoding"),
       ITEM("TE"),
       ITEM("Expect"),
       ITEM("Upgrade"),
       ITEM("Accept-Encoding"),
       ITEM("Via"),
       ITEM("Authorization"),
       ITEM("Keep-Alive"),
       ITEM("X-Forward-For"),
       ITEM("X-Real-IP"),
       ITEM("Accept"),
       ITEM("Accept-Language"),
       ITEM("Depth"),
       ITEM("Destination"),
       ITEM("Overwrite"),
       ITEM("Date"),
       ITEM("Cookie"),
#undef ITEM
};

enum HTTPCoreHeaderOut {
    HO_COMMON = -1,
    HO_HOST = 0,
};
} // namespace HTTP
} // namespace ngx

#endif //LIBNGX_COMMON_HEADER_H
