#include "control/commander.h"
//    PI_Controller(double sampling_time,
//                  double gain_proportional,
//                  double time_constant_inverse,
//                  double gain_correction,
//                  double feed_fwd,
//                  double limit_lower,
//                  double limit_upper ):


Commander::Commander(Crazyflie & crazyflie) :
    _crazyflie(crazyflie),
  _hoverModeIsActive(false),
  _piYaw (sampling_time, 1.0f, 0.0f, 0.0f, 0.0f, -180.0f,180.0f),
  _piRoll (sampling_time, 1.0f, 0.0f, 0.0f, 0.0f, -180.0f,180.0f),
  _piPitch(sampling_time, 1.0f, 0.0f, 0.0f, 0.0f, -180.0,180.0f),
  _zAcceleration(sampling_time, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f,10.0f) // What is the unit of acc z?
{}

void Commander::Update()
{
    if(_hoverModeIsActive)
    {
        SetPoint setPoint;
        auto const & sensorValues = _crazyflie.GetSensorValues();
        setPoint.yaw = sensorValues.stabilizer.yaw;
//        setPoint.yaw = _piYaw.Update (-sensorValues.stabilizer.yaw);
        setPoint.roll = _piRoll.Update (-sensorValues.stabilizer.roll);
        setPoint.pitch = _piPitch.Update (-sensorValues.stabilizer.pitch);
        setPoint.thrust = 36000;
        std::cout << "Actual Values: ";
        std::cout << "roll = " << sensorValues.stabilizer.roll << " pitch = " << sensorValues.stabilizer.pitch << " yaw = " << sensorValues.stabilizer.yaw << " thrust = " << sensorValues.stabilizer.thrust << std::endl;
        std::cout << "Reference: ";
        setPoint.Print();
        std::cout << "----------------------------------------\n";
        _crazyflie.SetSetPoint(setPoint);
        _crazyflie.SetSendSetpoints(true);
    }

}
