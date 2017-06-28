#pragma once
#include "gtest/gtest.h"
#include "math/functions.h"
#include "math/constants.h"

class Math_Function_Test : public testing::Test
{

};


TEST_F(Math_Function_Test, WrapAround)
{
    EXPECT_EQ(4, WrapAround(0, 1 ,5));
    EXPECT_EQ(5, WrapAround(1, 1 ,5));
    EXPECT_EQ(5, WrapAround(5, 1 ,5));
    EXPECT_EQ(2, WrapAround(-2, 1 ,5));
    EXPECT_FLOAT_EQ(0.0, WrapAround(0.0, -180.0 ,180.0));
    EXPECT_FLOAT_EQ(90.0, WrapAround(-270.0, -180.0 , 180.0));
    EXPECT_FLOAT_EQ(180.0, WrapAround(180.0, -180.0 , 180.0));
    EXPECT_FLOAT_EQ(180.0, WrapAround(-180.0, -180.0 , 180.0));
    EXPECT_FLOAT_EQ(0.0, WrapAround(-360.0, -180.0 , 180.0));
    EXPECT_FLOAT_EQ(0.0, WrapAround(-720.0, -180.0 , 180.0));
    EXPECT_FLOAT_EQ(0.0, WrapAround(pi2, -pi , pi));
    EXPECT_FLOAT_EQ(-pi + 0.1, WrapAround(pi+0.1f, -pi , pi));
    EXPECT_FLOAT_EQ(pi-0.1, WrapAround(-pi-0.1f, -pi , pi));
}