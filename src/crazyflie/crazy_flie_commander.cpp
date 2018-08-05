#include "crazyflie/crazy_flie_commander.h"
#include "math/constants.h"
#include "math/types.h"

static constexpr float limit = 2.0;

CrazyFlieCommander::CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime) :
    _crazyflie(crazyflie),
    _samplingTime(samplingTime),
    //(sampling_time,   gain_proportional, time_constant_inverse, gain_correction,  feed_fwd, limit_lower,limit_upper ):
    _piXVelocity (samplingTime, 0.1f,  0.02f, 1.0f, 0.0f, -limit/4.0,limit/4.0), // in meter
    _piYVelocity (samplingTime, 0.1f,  0.02f, 1.0f, 0.0f, -limit/4.0,limit/4.0), // in meter
    _piZVelocity (samplingTime, 2.0f,   0.1f, 1.0f, 0.0f, -limit,limit), // in meter
    _currentEstimate(),
    _takeOffTimeTicks(std::round(0.5/samplingTime)),
    _landingTimeTicks(std::round(1.5/samplingTime))
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
        else if(_waitCameraCntr == std::round(0.5/_samplingTime)) // wait for 500 ms
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
            // Start with z-velocity 0.5m/s and gradually decrease to 0.3 in the time given
            Velocity velocity;
            velocity[0] = 0.0;
            velocity[1] = 0.0;
            velocity[2] = 0.5*(_takeOffCntr/_takeOffTimeTicks) + 0.3;
            _crazyflie.SetVelocityRef(velocity);
            _crazyflie.SetSendingVelocityRef(true);

            ++_takeOffCntr;
            if( _takeOffCntr == _takeOffTimeTicks)
            {
                _flightState = FlightState::Follow;
                //                ImmediateStop();
                //                _flightState = FlightState::Off;
                //                _enableHover = false;
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
            //            UpdateHoverMode();

            Velocity velocity;
            velocity[0] = 0.0;
            velocity[1] = 0.0;
            velocity[2] = 0.08; // Use this as feedforward for pid!

            _crazyflie.SetVelocityRef(velocity);
            _crazyflie.SetSendingVelocityRef(true);


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
            velocity[2] = 0.05;
            _crazyflie.SetVelocityRef(velocity);
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

void CrazyFlieCommander::ResetVelocityController(float z_integral_part, float y_integral_part, float x_integral_part)
{
    _piXVelocity.Reset(x_integral_part);
    _piYVelocity.Reset(y_integral_part);
    _piZVelocity.Reset(z_integral_part);
}

void CrazyFlieCommander::UpdateHoverMode()
{

    Point3f const & currentEstimate = _currentEstimate.read();
    Velocity velocity;
    velocity[0] = _piXVelocity.Update(currentEstimate.x - 0.5); // is in meter ! The ball should be 0.5 m away from the crazyflie
    velocity[1] = _piYVelocity.Update(currentEstimate.y); // is in meter
    velocity[2] = _piZVelocity.Update(currentEstimate.z); // is in meter

    textLogger << "Distance error, x = " <<  (currentEstimate.x - 0.5) << " y = " << currentEstimate.y << " z = "<< currentEstimate.z << "\n";
    //    textLogger << "PI velocity outputs, x = " << velocity[0] << " y = " << velocity[1] << " z = " << velocity[2] << "\n";
    _crazyflie.SetVelocityRef(velocity);
    _crazyflie.SetSendingVelocityRef(true);
}
void CrazyFlieCommander::ImmediateStop()
{
    _crazyflie.SetSetPoint({0,0,0,0});
    _crazyflie.SetSendSetpoints(true);
    _crazyflie.SetVelocityRef({0,0,0});
    _crazyflie.SetSendingVelocityRef(false);
    _crazyflie.Stop();
}
