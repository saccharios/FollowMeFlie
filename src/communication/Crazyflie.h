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

enum class State {
    STATE_ZERO = 0,
    STATE_READ_PARAMETERS_TOC = 1,
    STATE_READ_LOGS_TOC = 2,
    STATE_START_LOGGING = 3,
    STATE_ZERO_MEASUREMENTS = 4,
    STATE_NORMAL_OPERATION = 5
};

class Crazyflie {

public:
    struct SetPoint
    {
        float roll;
        float pitch;
        float yaw;
        uint16_t thrust;
    };

    Crazyflie(CrazyRadio & crazyRadio);
    ~Crazyflie();

    // The thrust value to send (> 10000)
    void SetThrust(int thrust);
    int GetThrust();

    void SetRoll(float roll);
    //    Roll values are in degree, ranging from -180.0deg to 180.0deg.
    float GetRoll();

    void SetPitch(float pitch);
    //    Pitch values are in degree, ranging from -180.0deg to 180.0deg.
    float GetPitch();


    void SetYaw(float yaw);
    //    Yaw values are in degree, ranging from -180.0deg to 180.0deg.
    float GetYaw();


    void EnableStateMachine(bool enable);
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
    bool Update();

    bool IsCopterConnected();

    bool IsInitialized();

    void SetSendSetpoints(bool sendSetpoints);

    bool IsSendingSetpoints();

    //Read back a sensor value you subscribed to

    //    Possible sensor values might be:
    //    * stabilizer.yaw
    //    * stabilizer.roll
    //    * stabilizer.pitch
    //    * pm.vbat

    //    The possible key names strongly depend on your firmware. If you
    //    don't know what to do with this, just use the convience functions
    //    like getRoll(), getPitch(), getYaw(), and batteryLevel().

    double GetSensorValue(std::string name);

    double GetBatteryLevel();
    float AccX();
    float AccY();
    float AccZ();
    float AccZW();
    float Asl();
    float AslLong();
    float Temperature();
    float Pressure();
    float GyroX();
    float GyroY();
    float GyroZ();
    float GetBatteryState();
    float MagX();
    float MagY();
    float MagZ();

private:
    CrazyRadio & _crazyRadio;

    int _ackMissTolerance;
    int _ackMissCounter;

    SetPoint _sendSetPoint;
    SetPoint _maxSetPoint;

    int _minThrust;

    bool _isSendingSetpoints;

    bool _stateMachineIsEnabled;
    State _state;

    TOC _tocParameters;
    TOC _tocLogs;

    bool ReadTOCParameters();
    bool ReadTOCLogs();

    bool SendSetpoint(SetPoint setPoint);

    void DisableLogging();

    void EnableStabilizerLogging();
    void EnableGyroscopeLogging();
    void EnableAccelerometerLogging();

    void DisableStabilizerLogging();
    void DisableGyroscopeLogging();
    void DisableAccelerometerLogging();

    void EnableBatteryLogging();
    void DisableBatteryLogging();

    void StartLogging();
    void StopLogging();

    void EnableMagnetometerLogging();
    void DisableMagnetometerLogging();

    void EnableAltimeterLogging();
    void DisableAltimeterLogging();

};

