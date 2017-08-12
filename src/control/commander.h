#pragma once

#include "math/pi_controller.h"
#include "communication/Crazyflie.h"




class Commander
{
    float sampling_time = 10.0; // TODO Generalize with crazyfliecaller
public:
    Commander(Crazyflie & crazyflie);



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

    PI_Controller _piYaw;
    PI_Controller _piRoll;
    PI_Controller _piPitch;
    PI_Controller _zAcceleration;



};
