#pragma once
#include "gtest/gtest.h"
#include "math/functions.h"
#include "math/constants.h"
#include "math/pi_controller.h"
#include "math/lowpass.h"

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

TEST_F(Math_Function_Test, Limit)
{
    EXPECT_EQ(1, Limit(1,-2,2));
    EXPECT_EQ(2, Limit(3,-2,2));
    EXPECT_EQ(-2, Limit(-4,-2,2));
    EXPECT_EQ(1.0, Limit(1.0,-2.0,1.1));
    EXPECT_EQ(2.5, Limit(3.0,-2.0,2.5));
    EXPECT_EQ(-1.5, Limit(-4.0,-1.5,1.0));
}


TEST_F(Math_Function_Test, PI_Controller)
{
    double sampling_time  = 0.001; // 1ms
    double gain_proportional = 0.1;
    double time_constant_inverse = 40.0;
    double gain_correction = 1.0;
    double feed_fwd = 0.0;
    double limit_lower = 0.0;
    double limit_upper = 0.50;

    PI_Controller pi_controller(sampling_time, gain_proportional, time_constant_inverse, gain_correction, feed_fwd, limit_lower, limit_upper);

    double  reference = 1.0;
    double actual = 0.0;
    double actual_old = 0.0;
    for(int i = 0; i < 50; ++i)
    {
        std::cout << i <<" " <<actual << std::endl;
        auto error = reference - actual;
        actual = pi_controller.Update(error);

        // Check antiwindup
        if(i == 20)
        {
            reference = 0.0;

        }
        else if( i == 21)
        {
            EXPECT_LT(actual, actual_old);
        }
        actual_old = actual;
    }
}

TEST_F(Math_Function_Test, LowPass)
{
    double sampling_time = 0.001;
    double time_constant = 0.1;
    Lowpass lowpass(sampling_time, time_constant);

    double reference = 1.0;
    for(int i = 1; i < 50; ++i)
    {
        auto actual = lowpass.Update(reference);
        double value = reference * (1.0 - exp(-i*sampling_time / time_constant));
        EXPECT_NEAR(actual, value, 0.0015) << "Step " << i;
    }

}

