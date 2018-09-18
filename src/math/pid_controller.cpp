#include "pid_controller.h"


float
PID_Controller::Update(float error)
{
    float pi_output = _pi_controller.Update(error);
    float derivative_part = (error - _previous_error)/_sampling_time * _gain_derivative;

    _previous_error = error;
    return pi_output + derivative_part;
}

void PID_Controller::Reset(float integral_part)
{
    _pi_controller.Reset(integral_part);
}
