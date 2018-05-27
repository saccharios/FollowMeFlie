#pragma once
#include "QOBJECT"
#include "math/pi_controller.h"
#include "crazyflie/crazy_flie.h"
#include "math/types.h"
#include "math/double_buffer.h"
#include "math/delay.h"

class CrazyFlieCommander :public QObject
{
    Q_OBJECT
public:
    CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime);

    void ActivateHoverMode()
    {
        _hoverModeIsActive.Activate(true);
    }

    void Update();
    void ResetVelocityController();

    void Stop()
    {
        _hoverModeIsActive.Activate(false);
        _crazyflie.SetSetPoint({0,0,0,0});
        _crazyflie.SetSendSetpoints(true);
        _crazyflie.SetVelocityRef({0,0,0});
        _crazyflie.SetSendingVelocityRef(false);

        _crazyflie.Stop();
    }

public slots:
    void ReceiveEstimate(Distance const &);

private:
    Crazyflie & _crazyflie;
    OnDelay<50> _hoverModeIsActive;
    float _samplingTime;

    PI_Controller _piXVelocity;
    PI_Controller _piYVelocity;
    PI_Controller _piZVelocity;
    Double_Buffer<Distance> _currentEstimate;

    void UpdateHoverMode();

};
