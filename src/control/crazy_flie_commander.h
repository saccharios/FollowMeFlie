#pragma once

#include "math/pi_controller.h"
#include "communication/crazy_flie.h"




class CrazyFlieCommander
{
public:
    CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime);

    void ActivateHoverMode(bool activate)
    {
        _hoverModeIsActive  = activate;
    }

    void Update();

    void Stop()
    {
        _hoverModeIsActive= false;
        _crazyflie.SetSetPoint({0,0,0,0});
        _crazyflie.SetSendSetpoints(true);
    }

private:
    Crazyflie & _crazyflie;
    bool _hoverModeIsActive;
    float _samplingTime;

    PI_Controller _piYaw;
    PI_Controller _piRoll;
    PI_Controller _piPitch;
    PI_Controller _zAcceleration;
};
