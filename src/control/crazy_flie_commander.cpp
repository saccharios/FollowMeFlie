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
        auto const & sensorValues = _crazyflie.GetSensorValues();

        Acceleration acceleration_body_frame = {sensorValues.acceleration.x, sensorValues.acceleration.y, sensorValues.acceleration.z};


        Acceleration acceleration_intertial_frame = _crazyflie.ConvertBodyFrameToIntertialFrame(acceleration_body_frame);


        std::cout << "acc_x_i = " << acceleration_intertial_frame[0] << " acc_y_i = " << acceleration_intertial_frame[1] << " acc_z_i = " << acceleration_intertial_frame[2] << std::endl;

        velocity += acceleration_intertial_frame * _samplingTime;;
        std::cout << "v_x = " << velocity[0] << " v_y = " << velocity[1] << " v_z = " << velocity[2] << std::endl;

        // In python client, this line implementes the x-mode
        float vx = -1.0f*velocity[0];
        float vy = -1.0f* velocity[1];
        float vz = -1.0f* velocity[2];
//        auto vxx = (vx-vy)*SQRT2;
//        auto vyy = (vx+vy)*SQRT2;
//        _crazyflie.SetVelocityRef(Velocity{vx, vy, vz});
        _crazyflie.SetVelocityRef(Velocity{0.0, 0.0, 0.0});
        _crazyflie.SetSendingVelocityRef(true);
//        _crazyflie.SetSetPoint(SetPoint{0.0,0.0,0.0,35000});
//        _crazyflie.SetSendSetpoints(true);
    }
    else
    {
        velocity = {0.0f,0.0f,0.0f};
    }
}
