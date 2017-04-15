#ifndef SIMPLETEST_H
#define SIMPLETEST_H
#include "gtest/gtest.h"



class SimpleTest : public ::testing::Test
{
public:
    void simple_test();
private:
};

TEST_F(SimpleTest, Test1)
{
    simple_test();
}

#endif // SIMPLETEST_H
