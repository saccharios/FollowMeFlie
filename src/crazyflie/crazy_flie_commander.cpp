#include "crazyflie/crazy_flie_commander.h"
#include "math/constants.h"
#include "math/types.h"

static constexpr float limit = 0.1;

CrazyFlieCommander::CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime) :
    _crazyflie(crazyflie),
    _hoverModeIsActive(),
    _samplingTime(samplingTime),
    //(sampling_time,   gain_proportional, time_constant_inverse, gain_correction,  feed_fwd, limit_lower,limit_upper ):
    _piXVelocity (samplingTime, 0.2f,  0.0f, 1.0f, 0.0f, -limit,limit), // in meter
    _piYVelocity (samplingTime, 0.05f, 0.0f, 1.0f, 0.0f, -limit,limit), // in pixel, implicitly converted to meter
    _piZVelocity (samplingTime, 0.05f, 0.0f, 1.0f, 0.0f, -limit,limit), // in pixel, implicitly converted to meter
    _currentEstimate()
{}

// Periodically called
void CrazyFlieCommander::Update()
{
    static bool previousHoverModeIsActive;
    if(_hoverModeIsActive.Value())
    {
        if(!previousHoverModeIsActive)
        {
            // Entering hover mode new, reset the PI's.
            ResetVelocityController();
        }
        UpdateHoverMode();
    }
    else
    {
        _crazyflie.SetSendingVelocityRef(false);
        _crazyflie.SetSendSetpoints(false);
    }
    previousHoverModeIsActive = _hoverModeIsActive.Value();
}
// called when new estimate is ready
void CrazyFlieCommander::ReceiveEstimate(Distance const & distance)
{
    _currentEstimate.write() = distance;
    _currentEstimate.swap();
}

void CrazyFlieCommander::ResetVelocityController()
{
    _piXVelocity.Reset();
    _piYVelocity.Reset();
    _piZVelocity.Reset();
}

void CrazyFlieCommander::UpdateHoverMode()
{

    Distance const & currentEstimate = _currentEstimate.read();
    Velocity velocity;
    velocity[0] = _piXVelocity.Update(currentEstimate.x-0.5); // is in m ! The ball should be 0.5 m away from the crazyflie
//    velocity[1] = 0;
//    velocity[2] = 0;
    velocity[1] = _piYVelocity.Update(currentEstimate.y); // is in m
    velocity[2] = _piZVelocity.Update(currentEstimate.z); // is in m

    textLogger << "Distance error, x = " <<  (currentEstimate.x-0.5) << " y = " << currentEstimate.y << " z = "<< currentEstimate.z << "\n";
    _crazyflie.SetVelocityRef(velocity);
    _crazyflie.SetSendingVelocityRef(true);
}
