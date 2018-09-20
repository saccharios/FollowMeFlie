#pragma once
#include "pi_controller.h"

class PID_Controller
{
public:
    PID_Controller(float sampling_time,
                   float gain_proportional,
                   float time_constant_inverse,
                   float gain_correction,
                   float feed_fwd,
                   float limit_lower,
                   float limit_upper,
                   float gain_derivative) :
        _pi_controller(sampling_time,
                       gain_proportional,
                       time_constant_inverse,
                       gain_correction,
                       feed_fwd,
                       limit_lower,
                       limit_upper),
        _gain_derivative(gain_derivative),
        _previous_error(),
        _sampling_time(sampling_time),
        _limit_lower(limit_lower),
        _limit_upper(limit_upper)
    {}
    float Update(float error);
    void Reset(float integral_part = 0);

private:
    PI_Controller _pi_controller;
    float _gain_derivative;
    float _previous_error;
    float _sampling_time;
    float _limit_lower;
    float _limit_upper;
};
