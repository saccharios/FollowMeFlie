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

#pragma once


#include <cmath>
#include "CrazyRadio.h"
#include "TOC.h"
#include <memory>
#include "math/logic.h"

enum class State {
    ZERO = 0,
    READ_PARAMETERS_TOC = 1,
    READ_LOGS_TOC = 2,
    START_LOGGING = 3,
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

static int crazyflieUpdateSamplingTime = 10;

class Crazyflie {

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
    void SetVelocityRef(float vx, float vy, float vz);


    void SetSendSetpoints(bool sendSetpoints);
    bool IsSendingSetpoints();

    void SetSendingVelocityRef(bool isSendingVelocityRef);
    bool IsSendingVelocityRef();

    SensorValues const & GetSensorValues() const {return _sensorValues;}

    void StartConnecting(bool enable);
    //    Should be called during every 'cycle' of the main program using
    //    this class. Things like sensor reading processing, integral
    //    calculation and controller signal application are performed
    //    here. This function also triggers communication with the
    //    copter. Not calling it for too long will cause a disconnect from
    //    the copter's radio.
    //    Returns a boolean value denoting the current status of the
    //    radio dongle. If it returns 'false', the dongle was most likely
    //    removed or somehow else disconnected from the host machine. If it
    //    returns 'true', the dongle connection works fine.
    void Update();

    bool IsDisconnected();
    bool IsConnecting();
    bool IsConnected();


   bool IsConnectionTimeout();

   void ConvertBodyFrameToIntertialFrame(float x_b, float y_b, float z_b, float & x_i, float & y_i, float & z_i);

private:
    CrazyRadio & _crazyRadio;

    int _ackMissTolerance;
    int _ackMissCounter;

    SetPoint _sendSetPoint;
    SetPoint _maxSetPoint;

    float _vx, _vy,_vz;

    int _minThrust;

    bool _isSendingSetpoints;
    bool _isSendingVelocityRef;

    bool _startConnecting;
    State _state;

    TOC _tocParameters;
    TOC _tocLogs;

    Negative_Edge_Detector _leaveConnectingState;


    SensorValues _sensorValues;

    static constexpr float _frequency = 1000.0;

    bool ReadTOCParameters();
    bool ReadTOCLogs();

    bool SendSetpoint(SetPoint setPoint);

    bool SendVelocityRef(float vx, float vy, float vz);

    void StartLogging();
    void StopLogging();
    void DisableLogging();

    void EnableStabilizerLogging();
    void EnableGyroscopeLogging();
    void EnableAccelerometerLogging();

    void DisableStabilizerLogging();
    void DisableGyroscopeLogging();
    void DisableAccelerometerLogging();

    void EnableBatteryLogging();
    void DisableBatteryLogging();


    void EnableMagnetometerLogging();
    void DisableMagnetometerLogging();

    void EnableBarometerLogging();
    void DisableAltimeterLogging();

    void UpateSensorValues();
    double GetSensorValue(std::string name);


};

