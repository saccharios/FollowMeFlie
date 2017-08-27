#include "control/commander.h"
#include "math/constants.h"
#include "math/types.h"

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
    _samplingTime(static_cast<float>(crazyflieUpdateSamplingTime)*0.001f),
    _piYaw (_samplingTime*0.001f, 0.0f, 0.0f, 0.0f, 0.0f, -180.0f,180.0f),
    _piRoll (_samplingTime*0.001f, 0.0f, 0.2f, 0.0f, 0.0f, -180.0f,180.0f),
    _piPitch(_samplingTime*0.001f, 0.0f, 0.2f, 0.0f, 0.0f, -180.0,180.0f),
    _zAcceleration(_samplingTime*0.001f, 0.0f, 0.0f, 0.0f, 0.0f, 10.0f,10.0f) // What is the unit of acc z?
{}

void Commander::Update()
{
    if(_hoverModeIsActive)
    {
        auto const & sensorValues = _crazyflie.GetSensorValues();

        Acceleration acceleration_body_frame = {sensorValues.acceleration.x, sensorValues.acceleration.y, sensorValues.acceleration.z};


        Acceleration acceleration_intertial_frame = _crazyflie.ConvertBodyFrameToIntertialFrame(acceleration_body_frame);


//        std::cout << "acc_x_i = " << acceleration_intertial_frame[0] << " acc_y_i = " << acceleration_intertial_frame[1] << " acc_z_i = " << acceleration_intertial_frame[2] << std::endl;

        static Velocity velocity;
        velocity += acceleration_intertial_frame * _samplingTime;;
        std::cout << "v_x = " << velocity[0] << " v_y = " << velocity[1] << " v_z = " << velocity[2] << std::endl;

        //        _crazyflie.SetVelocityRef(0.0f,0.0f,0.0f);
        //        _crazyflie.SetSendingVelocityRef(true);
        //        _crazyflie.SetSetPoint(setPoint);
        //        _crazyflie.SetSendSetpoints(true);
    }

}
