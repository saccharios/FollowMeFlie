#pragma once
#include "QOBJECT"
#include "math/pid_controller.h"
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
        ResetKalman = 2,
        TakeOff = 3,
        Follow = 4,
        Landing = 5,
    };

    struct Commands
    {
            bool enableHover = false;
            bool emergencyStop = false;
    };
    void ActivateHoverMode(bool enable) {commands.enableHover = enable;}
    void EmergencyStop(bool enable) {commands.emergencyStop = enable;}


    void Update();
    void ResetVelocityController(float z_integral_part = 0, float y_integral_part = 0, float x_integral_part = 0);
    Point3f GetSetPoint() {return _setPoint;}


public slots:
    void ReceiveBallEstimate(Point3f const &);
    void SetCameraIsRunning(bool const &);
    void SetSetPoint(Point3f setPoint) {_setPoint = setPoint;}

private:
    Crazyflie & _crazyflie;
    float _samplingTime;

    PID_Controller _pid_ZVelocity;
    Double_Buffer<Point3f> _currentBallEstimate;
    FlightState _flightState = FlightState::Off;
    int _takeOffCntr = 0;
    int _takeOffTimeTicks;
    int _landingCntr = 0;
    int _landingTimeTicks;
    Commands commands;
    bool _cameraIsRunning = false;
    Point3f _setPoint{0.5, 0.0, 0.0};


    Velocity UpdateHoverMode();
    void ImmediateStop();
};
