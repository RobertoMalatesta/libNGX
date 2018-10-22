#include "Core/Core.h"

using namespace std;
using namespace ngx::Core;

int HashTest() {

    u_char test_case[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 2; i < sizeof(test_case); i++) {
        uint32_t hash = murmur_hash2(test_case, (size_t) i - 1);
        printf("%08X\n", hash);
    }
    return 0;
}
