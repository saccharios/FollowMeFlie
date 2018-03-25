#include "crazyflie/crazy_flie_commander.h"
#include "math/constants.h"
#include "math/types.h"

static constexpr float limit = 0.1;

CrazyFlieCommander::CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime) :
    _crazyflie(crazyflie),
    _hoverModeIsActive(false),
    _samplingTime(samplingTime),
    //(sampling_time,   gain_proportional, time_constant_inverse,gain_correction,feed_fwd,limit_lower,limit_upper ):
    _piXVelocity (samplingTime*0.001f, 0.2f, 0.1f, 1.0f, 0.0f, -limit,limit),
    _piYVelocity (samplingTime*0.001f, 0.05f, 0.1f, 1.0f, 0.0f, -limit,limit),
    _piZVelocity(samplingTime*0.001f, 0.05f, 0.1f, 1.0f, 0.0f, -limit,limit),
    _currentEstimate()
{}

// Periodically called
void CrazyFlieCommander::Update()
{
    if(_hoverModeIsActive)
    {
        UpdateHoverMode();
    }
    else
    {
        _crazyflie.SetSendingVelocityRef(false);
        _crazyflie.SetSendSetpoints(false);
    }
}
// called when new estimate is ready
void CrazyFlieCommander::ReceiveEstimate(Distance const & distance)
{
    _currentEstimate.write() = distance;
    _currentEstimate.swap();
}
void CrazyFlieCommander::UpdateHoverMode()
{

    Distance const & currentEstimate = _currentEstimate.read();
    Velocity velocity;
    velocity[0] = _piXVelocity.Update(-currentEstimate.x); // is in cm !
//    velocity[1] = 0;
//    velocity[2] = 0;
    velocity[1] = _piYVelocity.Update(-currentEstimate.y);
    velocity[2] = _piZVelocity.Update(-currentEstimate.z);

    _crazyflie.SetVelocityRef(velocity);
    _crazyflie.SetSendingVelocityRef(true);
}
