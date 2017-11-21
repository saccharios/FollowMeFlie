
// With the newest firmware for the crazyflie 2.0, the motor need to be unlocked by sending a "thrust = 0" command
// Update SF: I disabled the locking-functionality in the firmware.

#pragma once

#include "QOBJECT"

#include <cmath>
#include "radio_dongle.h"
#include <memory>
#include "math/logic.h"
#include "math/types.h"
#include "toc_log.h"
#include "toc_parameter.h"

enum class State
{
    ZERO = 0,
    SETUP_PARAMETERS = 1,
    READ_PARAMETERS = 2,
    SETUP_LOGGER = 3,
    ZERO_MEASUREMENTS = 4,
    NORMAL_OPERATION = 5
};

struct SetPoint
{
    float roll;
    float pitch;
    float yaw;
    uint16_t thrust;
    void Print()
    {
        std::cout << "roll = " << roll << " pitch = " << pitch << " yaw = " << yaw << " thrust = " << thrust << std::endl;
    }
};
struct Barometer
{
    float asl;
    float aslLong;
    float temperature;
    float pressure;
};
struct Accelerometer
{
    float x;
    float y;
    float z;
    float zw;
};

struct Gyrometer
{
    float x;
    float y;
    float z;
};
struct Battery
{
    double  level;
    float      state;
};

struct Magnetometer
{
    float x;
    float y;
    float z;
};
struct SensorValues
{
    SetPoint             stabilizer;
    Barometer          barometer;
    Accelerometer   acceleration;
    Gyrometer          gyrometer;
    Battery                battery;
    Magnetometer    magnetometer;
};

class Crazyflie : public QObject
{
    Q_OBJECT

public:


    Crazyflie(RadioDongle & _radioDongle);
    ~Crazyflie();

    void SetThrust(int thrust);
    void SetRoll(float roll); // In degree, -180° to 180°
    void SetPitch(float pitch); // In degree, -180° to 180°
    void SetYaw(float yaw); // In degree, -180° to 180°

    void SetSetPoint(SetPoint setPoint);
    void SetVelocityRef(Velocity velocity);


    void SetSendSetpoints(bool sendSetpoints);
    bool IsSendingSetpoints();

    void SetSendingVelocityRef(bool isSendingVelocityRef);
    bool IsSendingVelocityRef();

    SensorValues const & GetSensorValues() const {return _sensorValues;}

    void StartConnecting(bool enable);

    void Update();

    bool IsDisconnected();
    bool IsConnecting();
    bool IsConnected();


   Eigen::Vector3f ConvertBodyFrameToIntertialFrame(Eigen::Vector3f const & value_in_body);

   std::vector<TOCElement> const &
   GetLogElements () const
   {
       return _logger.GetElements();
   }

   std::vector<TOCElement> const &
   GetParameterElements() const
   {
       return _parameters.GetElements();
   }

   TocParameter const &
   GetParameterTOC() const
   {
       return _parameters;
   }


signals:
    void ConnectionTimeout();
    void NotConnecting();

private:
    RadioDongle & _radioDongle;

    int _ackMissTolerance;
    int _ackMissCounter;

    SetPoint _sendSetPoint;
    SetPoint _maxSetPoint;

    Velocity _velocity = {};
    Acceleration _accelerationOffset = {};
    SetPoint _setPointOffset = {};


    int _minThrust;

    bool _isSendingSetpoints;
    bool _isSendingVelocityRef;

    bool _startConnecting;
    State _state;

    TocParameter _parameters;
    TocLog _logger;

    Negative_Edge_Detector _leaveConnectingState;

    SensorValues _sensorValues;

    static constexpr float _frequency = 1000.0f; // 1000.0

    bool SendSetpoint(SetPoint setPoint);

    bool SendVelocityRef(Velocity velocity);

    void UpateSensorValues();
    float GetSensorValue(std::string name);

    bool RegisterLoggingBlocks();
    void RegisterLogginBlock(bool & success, std::string name, float frequency);

    bool StartLogging();
    void StopLogging();
    void DisableLogging();

    void EnableSensorsLogging();
    void DisableSensorsLogging();

    void EnableBatteryLogging();
    void DisableBatteryLogging();

    void EnablePIDAttitudeLogging();
    void DisablePIDAttitudeLogging();
    void EnablePIDRateLogging();
    void DisablePIDRateLogging();
    void EnableControllerLogging();
    void DisableControllerLogging();
    void EnableKalman1Logging();
    void DisableKalman1Logging();
    void EnableKalman2Logging();
    void DisableKalman2Logging();
    void EnablePosCtrlLogging();
    void DisablePosCtrlLogging();
    void EnableAltitudeEstimationLogging();
    void DisableAltitudeEstimationLogging();
    void EnableMotorsLogging();
    void DisableMotorsLogging();
    void EnableSensorFusionLogging();
    void DisableSensorFusionLogging();
    void EnableCtrlTargetLogging();
    void DisableCtrlTargetLogging();
    void EnableStateEstimateLogging();
    void DisableStateEstimateLogging();

};

