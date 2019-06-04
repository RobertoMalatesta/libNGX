#include "Core/Support/Hash.h"
#include <iostream>
#include <gtest/gtest.h>
using namespace std;
using namespace ngx::Core::Support;

TEST(Support, HashTest) {
    u_char test_case[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    StringRef s(test_case, sizeof(test_case)-1);
    cout<< murmurhash2(s, false) << endl;
}
