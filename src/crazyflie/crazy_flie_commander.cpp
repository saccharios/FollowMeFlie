#include "crazyflie/crazy_flie_commander.h"
#include "math/constants.h"
#include "math/types.h"
#include <chrono>
static constexpr float limit = 0.5;

CrazyFlieCommander::CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime) :
    _crazyflie(crazyflie),
    _samplingTime(samplingTime),
    //(sampling_time,   gain_proportional, time_constant_inverse, gain_correction,  feed_fwd, limit_lower,limit_upper ):
    _piXVelocity (samplingTime, 0.3f,  0.001f, 1.0f, 0.0f, -limit,limit), // in meter
    _piYVelocity (samplingTime, 0.3f,  0.001f, 1.0f, 0.0f, -limit,limit), // in meter
    _piZVelocity (samplingTime, 1.0f,   0.001f, 1.0f, 0.08f, -limit*2.0f,limit*2.0f), // in meter
    _currentEstimate(),
    _takeOffTimeTicks(std::round(0.8f/samplingTime)),
    _landingTimeTicks(std::round(1.5f/samplingTime))
{}

// Periodically called
void CrazyFlieCommander::Update()
{
    //    std::cout << "flight state = " << static_cast<int>(_flightState) << std::endl;
    switch(_flightState)
    {
    case FlightState::Off:
    {
        if(commands.enableHover)
        {
            _flightState = FlightState::WaitCameraOn;
            _waitCameraCntr = 0;
            _crazyflie.ResetCrazyflieKalmanFilter(true);
        }
        break;
    }
    case FlightState::WaitCameraOn:
    {
        ++_waitCameraCntr;
        if(!commands.enableHover)
        {
            ImmediateStop();
            _flightState = FlightState::Off;
        }
        else if(_waitCameraCntr == std::round(0.5f/_samplingTime)) // wait for 500 ms
        {
            // Wait for 500 ms, camera should be on
            // TODO SF: Wait until camera is actuall on, instead of waiting 50 ticks
            ResetVelocityController();
            _crazyflie.ResetCrazyflieKalmanFilter(false);
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
        else
        {
            // Start with z-velocity 1.0m/s and gradually decrease to 0.3 in the time given
            Velocity velocity;
            velocity[0] = 0.0;
            velocity[1] = 0.0;
            velocity[2] = 0.6f*(_takeOffCntr/_takeOffTimeTicks) + 0.3f;
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
        else
        {
            bool use_velocity = true;
            if(use_velocity)
            {
                Velocity velocity = UpdateHoverMode();
                _crazyflie.SetVelocityCrazyFlieRef(velocity);
                _crazyflie.SetSendingVelocityRef(true);
            }
            else
            //Use with external positioning method
            {
                Point3f position_act = UpdateHoverMode_Position();
                Point3f position_ref = {0.5,0.0,0.0};
                _crazyflie.SetPositionSetPoint(position_ref,position_act);
                _crazyflie.SetSendPositionSetPoint(true);
            }



        }
        break;
    }
    case FlightState::Landing:
    {
        if(commands.emergencyStop || _landingCntr == _landingTimeTicks)
        {
            ImmediateStop();
            _flightState = FlightState::Off;
            commands.enableHover = false;
            commands.emergencyStop = false;
        }
        else
        {
            Velocity velocity;
            velocity[0] = 0.0;
            velocity[1] = 0.0;
            velocity[2] = 0.05f;
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
    std::cout << "new estimate is ready\n";
    _currentEstimate.write() = distance;
    _currentEstimate.swap();
}

void CrazyFlieCommander::ResetVelocityController(float z_integral_part, float y_integral_part, float x_integral_part)
{
    _piXVelocity.Reset(x_integral_part);
    _piYVelocity.Reset(y_integral_part);
    _piZVelocity.Reset(z_integral_part);
}

Velocity CrazyFlieCommander::UpdateHoverMode()
{

    Point3f const & currentEstimate = _currentEstimate.read(); // is in meter
    Velocity velocity;
    Point3f error = currentEstimate;
    error.x -= 0.5f; // The ball should be 0.5 m away from the crazyflie
    velocity[0] = _piXVelocity.Update(error.x);
    velocity[1] = _piYVelocity.Update(error.y);
    velocity[2] = _piZVelocity.Update(error.z);


    std::cout << "Distance error, x = " <<  error.x << " y = " << error.y << " z = "<< error.z << "\n";
    std::cout << "PI velocity outputs, x = " << velocity[0] << " y = " << velocity[1] << " z = " << velocity[2] << "\n";
    return velocity;
}
Point3f CrazyFlieCommander::UpdateHoverMode_Position()
{
    Point3f const & currentEstimate = _currentEstimate.read();
    return currentEstimate;
}

void CrazyFlieCommander::ImmediateStop()
{
    _crazyflie.SetSetPoint({0,0,0,0});
    _crazyflie.SetSendSetpoints(true);
    _crazyflie.SetVelocityCrazyFlieRef({0,0,0});
    _crazyflie.SetSendingVelocityRef(false);
    _crazyflie.Stop();
}
