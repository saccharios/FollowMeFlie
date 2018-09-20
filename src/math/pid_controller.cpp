#include "pid_controller.h"
#include "math/functions.h"

float
PID_Controller::Update(float error)
{
    float pi_output = _pi_controller.Update(error);
    float derivative_part = (error - _previous_error)/_sampling_time * _gain_derivative;

    _previous_error = error;
    auto output_limited = Limit(pi_output + derivative_part, _limit_lower, _limit_upper);
    return output_limited;
}

void PID_Controller::Reset(float integral_part)
{
    _pi_controller.Reset(integral_part);
    _previous_error = 0.0f;
}
