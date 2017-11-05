// Copyright (c) 2013, Jan Winkler <winkler@cs.uni-bremen.de>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Universität Bremen nor the names of its
//       contributors may be used to endorse or promote products derived from
//       this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


// \author Jan Winkler


// With the newest firmware for the crazyflie 2.0, the motor need to be unlocked by sending a "thrust = 0" command
// Update SF: I disabled the locking-functionality in the firmware.


#pragma once

#include "QOBJECT"

#include <cmath>
#include "CrazyRadio.h"
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
    double level;
    float state;
};

struct Magnetometer
{
    float x;
    float y;
    float z;
};
struct SensorValues
{
    SetPoint stabilizer;
    Barometer barometer;
    Accelerometer acceleration;
    Gyrometer gyrometer;
    Battery battery;
    Magnetometer magnetometer;
};

static int crazyflieUpdateSamplingTime = 10; // in ms

class Crazyflie : public QObject
{
    Q_OBJECT

public:


    Crazyflie(CrazyRadio & crazyRadio);
    ~Crazyflie();

    // The thrust value to send (> 10000)
    void SetThrust(int thrust);
    //    Roll values are in degree, ranging from -180.0deg to 180.0deg.
    void SetRoll(float roll);
    //    Pitch values are in degree, ranging from -180.0deg to 180.0deg.
    void SetPitch(float pitch);
    //    Yaw values are in degree, ranging from -180.0deg to 180.0deg.
    void SetYaw(float yaw);

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


   std::array<float,3> ConvertBodyFrameToIntertialFrame(std::array<float,3> const & value_in_body);

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
    CrazyRadio & _crazyRadio;

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

