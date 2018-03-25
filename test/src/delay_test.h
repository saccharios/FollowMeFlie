#pragma once
#include "gtest/gtest.h"
#include "math/delay.h"


class DelayTest : public testing::Test
{


};
TEST_F(DelayTest, OnDelay)
{
    OnDelay<5> delay;
    delay.Activate(false);
    EXPECT_FALSE(delay.Value());
    delay.Activate(true);
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_TRUE(delay.Value());
    EXPECT_TRUE(delay.Value());
    delay.Activate(false);
    EXPECT_FALSE(delay.Value());
    delay.Activate(true);
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    delay.Activate(false);
    EXPECT_FALSE(delay.Value());
    delay.Activate(true);
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_FALSE(delay.Value());
    EXPECT_TRUE(delay.Value());

}
