#include "crazyflie/crazy_flie_commander.h"
#include "math/constants.h"
#include "math/types.h"

CrazyFlieCommander::CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime) :
    _crazyflie(crazyflie),
    _samplingTime(samplingTime),
    //(sampling_time,   gain_proportional, time_constant_inverse, gain_correction,  feed_fwd, limit_lower,limit_upper ):
    _currentEstimate(),
    _takeOffTimeTicks(static_cast<int>(std::round(0.8f/samplingTime))),
    _landingTimeTicks(static_cast<int>(std::round(2.0f/samplingTime)))
{}

// Periodically called
void CrazyFlieCommander::Update()
{
    //    std::cout << "flight state = " << static_cast<int>(_flightState) << std::endl;
    bool emergencyStopInternal = commands.emergencyStop || _crazyflie.IsGoneCrazy();

    switch(_flightState)
    {
    case FlightState::Off:
    {
        if(commands.enableHover)
        {
            _flightState = FlightState::WaitCameraOn;
            _crazyflie.ResetCrazyflieKalmanFilter(true);
        }
        break;
    }
    case FlightState::WaitCameraOn:
    {
        if(!commands.enableHover)
        {
            ImmediateStop();
        }
        else if(_cameraIsRunning)
        {
            _takeOffCntr = 0;
            _flightState = FlightState::TakeOff;
        }
        break;
    }
    case FlightState::TakeOff:
    {
        if(!commands.enableHover)
        {
            _landingCntr = 0;
            _flightState = FlightState::Landing;
        }
        else if(emergencyStopInternal)
        {
            ImmediateStop();
        }
        else
        {
            // Start with z-velocity 0.9m/s and gradually decrease to 0.3 in the time given
            Velocity velocity;
            velocity[0] = 0.0;
            velocity[1] = 0.0;
            velocity[2] = 0.5f*(_takeOffCntr/_takeOffTimeTicks) + 0.5f;
            _crazyflie.SetVelocityCrazyFlieRef(velocity);
            _crazyflie.SetSendingVelocityRef(true);
            ++_takeOffCntr;
            if( _takeOffCntr == _takeOffTimeTicks)
            {
                _flightState = FlightState::Follow;
            }

        }
        break;
    }
    case FlightState::Follow:
    {
        if(!commands.enableHover)
        {
            _landingCntr = 0;
            _flightState = FlightState::Landing;
        }
        else if(emergencyStopInternal)
        {
            ImmediateStop();
        }
        else
        {
            Velocity velocity = UpdateHoverMode();
            _crazyflie.SetVelocityCrazyFlieRef(velocity);
            _crazyflie.SetSendingVelocityRef(true);
        }
        break;
    }
    case FlightState::Landing:
    {
        if(emergencyStopInternal || _landingCntr == _landingTimeTicks)
        {
            ImmediateStop();
        }
        else
        {
            Velocity velocity;
            velocity[0] = 0.0;
            velocity[1] = 0.0;
            velocity[2] = -0.03f;
            _crazyflie.SetVelocityCrazyFlieRef(velocity);
            _crazyflie.SetSendingVelocityRef(true);
            ++_landingCntr;
        }

        break;
    }
    }
}
// called when new estimate is ready
void CrazyFlieCommander::ReceiveEstimate(Point3f const & distance)
{
    _currentEstimate.write() = distance;
    _currentEstimate.swap();
}

Velocity CrazyFlieCommander::UpdateHoverMode()
{

    Point3f const & currentEstimate = _currentEstimate.read(); // is in meter
    Velocity velocity;
    Point3f error = currentEstimate;
    error.x -= 0.5f; // The ball should be 0.5 m away from the crazyflie
    velocity[0] = error.x;
    velocity[1] = error.y;
    velocity[2] = error.z + 0.1f; // 0.1 is feed forward term
    //std::cout << "Distance error, x = " <<  error.x << " y = " << error.y << " z = "<< error.z << "\n";
    return velocity;
}

void CrazyFlieCommander::ImmediateStop()
{
    _crazyflie.SetVelocityCrazyFlieRef({0,0,0});
    _crazyflie.SetSendingVelocityRef(false);
    _crazyflie.Stop();
    _flightState = FlightState::Off;
    commands.enableHover = false;
    commands.emergencyStop = false;
}

void CrazyFlieCommander::SetCameraIsRunning(bool const & running)
{
    _cameraIsRunning = running;
}
