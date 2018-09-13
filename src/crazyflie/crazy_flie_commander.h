#pragma once
#include "QOBJECT"
#include "math/pi_controller.h"
#include "crazyflie/crazy_flie.h"
#include "math/types.h"
#include "math/double_buffer.h"

class CrazyFlieCommander :public QObject
{
    Q_OBJECT
public:
    CrazyFlieCommander(Crazyflie & crazyflie, float samplingTime);

    enum class FlightState
    {
        Off = 0,
        WaitCameraOn = 1,
        TakeOff = 2,
        Follow = 3,
        Landing = 4,
    };

    struct Commands
    {
            bool enableHover = false;
            bool emergencyStop = false;
    };
    void ActivateHoverMode(bool enable) {commands.enableHover = enable;}
    void EmergencyStop(bool enable) {commands.emergencyStop = enable;}


    void Update();

public slots:
    void ReceiveEstimate(Point3f const &);

private:
    Crazyflie & _crazyflie;
    float _samplingTime;

    Double_Buffer<Point3f> _currentEstimate;
    FlightState _flightState = FlightState::Off;
    int _waitCameraCntr = 0;
    int _takeOffCntr = 0;
    int _takeOffTimeTicks;
    int _landingCntr = 0;
    int _landingTimeTicks;
    Commands commands;
    Velocity UpdateHoverMode();
    void ImmediateStop();
};
