#include <gtest/gtest.h>

extern int CoreTest();

extern int HTTPTest();

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
