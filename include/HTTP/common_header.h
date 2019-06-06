#ifndef LIBNGX_COMMON_HEADER_H
#define LIBNGX_COMMON_HEADER_H
#include "common_header.h"
#include "Core/Support/MemoryBuffer.h"
#include "HTTP/config_header.h"
#include <vector>
namespace ngx {
namespace HTTP {
using namespace Config;
using namespace ngx::Core::Support;

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

} // namespace HTTP
} // namespace ngx

#endif //LIBNGX_COMMON_HEADER_H
