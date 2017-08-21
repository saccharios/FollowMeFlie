#include "control/commander.h"
#include "math/constants.h"
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
  _piYaw (crazyflieUpdateSamplingTime, 0.0f, 0.0f, 0.0f, 0.0f, -180.0f,180.0f),
  _piRoll (crazyflieUpdateSamplingTime, 0.0f, 0.2f, 0.0f, 0.0f, -180.0f,180.0f),
  _piPitch(crazyflieUpdateSamplingTime, 0.0f, 0.2f, 0.0f, 0.0f, -180.0,180.0f),
  _zAcceleration(crazyflieUpdateSamplingTime, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f,10.0f) // What is the unit of acc z?
{}

void Commander::Update()
{
    if(_hoverModeIsActive)
    {
        auto const & sensorValues = _crazyflie.GetSensorValues();

        auto acc_x_b = sensorValues.acceleration.x;
        auto acc_y_b = sensorValues.acceleration.y;
        auto acc_z_b = sensorValues.acceleration.z;

        float  acc_x_i;
        float  acc_y_i;
        float  acc_z_i;
        _crazyflie.ConvertBodyFrameToIntertialFrame(acc_x_b, acc_y_b, acc_z_b, acc_x_i,acc_y_i,acc_z_i);


        std::cout << "acc_x_i = " << acc_x_i << " acc_y_i = " << acc_y_i << " acc_z_i = " << acc_z_i << std::endl;
//        _crazyflie.SetVelocityRef(0.0f,0.0f,0.0f);
//        _crazyflie.SetSendingVelocityRef(true);
//        _crazyflie.SetSetPoint(setPoint);
//        _crazyflie.SetSendSetpoints(true);
    }

}
