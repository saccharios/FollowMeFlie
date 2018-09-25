#include "pid_controller.h"
#include "math/functions.h"

float
PID_Controller::Update(float error)
{
    float derivative_part = (error - _previous_error)/_sampling_time * _gain_derivative;
    _previous_error = error;
    return _pi_controller.Update(error,derivative_part);
}

void PID_Controller::Reset(float integral_part)
{
    _pi_controller.Reset(integral_part);
    _previous_error = 0.0f;
}
