#pragma once
#include "math/pid_controller.h"
#include "gtest/gtest.h"


class PID_Controller_Test : public ::testing::Test
{
public:
private:
};

TEST_F(PID_Controller_Test, P_Gain)
{
    float sampling_time = 0.001f;
    float gain_proportional = 0.5f;
    float time_constant_inverse = 0.0f;
    float gain_correction = 0.0;
    float feed_fwd = 0.1f;
    float limit_lower = -1.0;
    float limit_upper = 1.0;
    float gain_derivative = 0.0f;

    PID_Controller pid_controller(sampling_time,
                           gain_proportional,
                           time_constant_inverse,
                           gain_correction,
                           feed_fwd,
                           limit_lower,
                           limit_upper,
                           gain_derivative);

    // Step input
    float input = 1;
    for(unsigned int i = 0; i < 10; ++i)
    {
        float output = pid_controller.Update(input);
        EXPECT_FLOAT_EQ(input*gain_proportional + feed_fwd, output) << "at i = " << i;
    }
}

TEST_F(PID_Controller_Test, I_Gain)
{
    float sampling_time = 0.01;
    float gain_proportional =0.0f;
    float time_constant_inverse = 1.0f;
    float gain_correction = 1.0;
    float feed_fwd = 0.0f;
    float limit_lower = -1.0;
    float limit_upper = 0.5;
    float gain_derivative = 0.0f;

    PID_Controller pid_controller(sampling_time,
                                  gain_proportional,
                                  time_constant_inverse,
                                  gain_correction,
                                  feed_fwd,
                                  limit_lower,
                                  limit_upper,
                                  gain_derivative);

    // Step input
    float input = 1;
    float integral = 0.0;
    // Run until saturation is reached
    for(unsigned int i = 0; i < static_cast<unsigned int>(limit_upper/(sampling_time*time_constant_inverse)); ++i)
    {
        float output = pid_controller.Update(input);
        integral += input * sampling_time*time_constant_inverse;
        EXPECT_FLOAT_EQ(output, integral);
    }
    // Saturation is reached, the output must be limited
    for(unsigned int i = 0; i < 10; ++i)
    {
        float output = pid_controller.Update(input);
        EXPECT_FLOAT_EQ(output, limit_upper);
    }

    // The input changes, there should not be windup
    input = -1;
    for(unsigned int i = 0; i < 10; ++i)
    {
        float output = pid_controller.Update(input);
        integral += input * sampling_time*time_constant_inverse;
        EXPECT_NEAR(output, integral,0.000001f);
    }
}
TEST_F(PID_Controller_Test, D_Gain)
{
    float sampling_time = 0.01;
    float gain_proportional =0.0f;
    float time_constant_inverse = 0.0f;
    float gain_correction = 1.0;
    float feed_fwd = 0.0f;
    float limit_lower = -1.0;
    float limit_upper = 1.0;
    float gain_derivative = 0.5f;

    PID_Controller pid_controller(sampling_time,
                                  gain_proportional,
                                  time_constant_inverse,
                                  gain_correction,
                                  feed_fwd,
                                  limit_lower,
                                  limit_upper,
                                  gain_derivative);

    // Step input
    float input = 0.01;
    float input_previous = 0;
    for(unsigned int i = 0; i < 10; ++i)
    {
        float output = pid_controller.Update(input);
        float derivative = (input - input_previous)/sampling_time * gain_derivative;
        input_previous = input;
        EXPECT_FLOAT_EQ(output, derivative);
        std::cout << output << std::endl;
    }
}
