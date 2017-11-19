#include "control/crazy_flie_commander.h"
#include "math/constants.h"
#include "math/types.h"

//    PI_Controller(double sampling_time,
//                  double gain_proportional,
//                  double time_constant_inverse,
//                  double gain_correction,
//                  double feed_fwd,
//                  double limit_lower,
//                  double limit_upper ):


CrazyFlieCommander::CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime) :
    _crazyflie(crazyflie),
    _hoverModeIsActive(false),
    _samplingTime(samplingTime),
    _piYaw (_samplingTime*0.001f, 0.0f, 0.0f, 0.0f, 0.0f, -180.0f,180.0f),
    _piRoll (_samplingTime*0.001f, 0.0f, 0.2f, 0.0f, 0.0f, -180.0f,180.0f),
    _piPitch(_samplingTime*0.001f, 0.0f, 0.2f, 0.0f, 0.0f, -180.0,180.0f),
    _zAcceleration(_samplingTime*0.001f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f,10.0f) // What is the unit of acc z?
{}

void CrazyFlieCommander::Update()
{
    static Velocity velocity;
    if(_hoverModeIsActive)
    {
        _crazyflie.SetVelocityRef(Velocity{0.0, 0.0, 0.0});
        _crazyflie.SetSendingVelocityRef(true);
    }
    else
    {
        velocity = {0.0f,0.0f,0.0f};
    }
}
