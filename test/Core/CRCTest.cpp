#include "Core/Support/Checksum.h"
#include <cstdio>

using namespace std;
using namespace ngx::Core::Support;

int CRCTest() {

    u_char test_case[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    uint32_t code = crc(test_case, sizeof(test_case) - 1);
    printf("%08X\n", code);
    uint32_t crc32 = crc32_long(test_case, sizeof(test_case) - 1);
    printf("%08X\n", crc32);
    crc32_update(&crc32, test_case, sizeof(test_case) - 1);
    printf("%08X\n", crc32);
    return 0;
}
