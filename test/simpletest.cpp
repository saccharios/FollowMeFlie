#include "simpletest.h"
#include <iostream>

void SimpleTest::simple_test()
{
    int x = 1;
    x++;
    EXPECT_EQ(2,x);
    std::cout << "yoo\n";
}
