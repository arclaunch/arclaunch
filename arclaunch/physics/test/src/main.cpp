#include "gtest/gtest.h"

#include "plates_test.hpp"

int main(int argc, char **argv)
{
    reg();
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    return result;
};