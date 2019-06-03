#include "Core/ADT/SmallVector.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;
using namespace ngx::Core::ADT;

TEST(ADT, smallVector) {
    SmallVector<int, 32> v(30, 0);
    v.push_back(1);
    for (auto &i : v) {
        cout << i << endl;
    }
}