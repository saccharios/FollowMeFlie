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


#include "Crazyflie.h"
#include <chrono>
#include "math/clock_gettime.h"

Crazyflie::Crazyflie(CrazyRadio & crazyRadio) : _crazyRadio(crazyRadio)
{
    // Review these values
    _maxAbsRoll = 45.0f;
    _maxAbsPitch = _maxAbsRoll;
    _maxYaw = 180.0f;
    _maxThrust = 60000;
    _minThrust = 0;//15000;

    _roll = 0;
    _pitch = 0;
    _yaw = 0;
    _thrust = 0;

    _sendsSetpoints = false;

    // TODO Why are they dynamically allocated?
    _tocParameters = new TOC(_crazyRadio, Port::Parameters);
    _tocLogs = new TOC(_crazyRadio, Port::Log);

    _state = State::STATE_ZERO;

    _stateMachineIsEnabled = false;
}

Crazyflie::~Crazyflie()
{
    StopLogging();
}

bool Crazyflie::ReadTOCParameters()
{
    if(_tocParameters->RequestMetaData() )
    {
        if(_tocParameters->RequestItems())
        {
            return true;
        }
    }

    return false;
}

bool Crazyflie::ReadTOCLogs()
{
    auto meta_ok = _tocLogs->RequestMetaData();
    if(meta_ok)
    {
        auto req = _tocLogs->RequestItems();
        if(req)
        {
            return true;
        }
    }

    return false;
}

bool Crazyflie::SendSetpoint(float roll, float pitch, float yaw, short thrust)
{
    auto data = ConvertTouint8_tVect(roll);
    auto pitchVect = ConvertTouint8_tVect(-pitch); // Warning: Is negated here.
    auto yawVect = ConvertTouint8_tVect(yaw);
    auto thrustVect = ConvertTouint8_tVect(thrust);

    data.insert(data.end(), pitchVect.begin(), pitchVect.end());
    data.insert(data.end(), yawVect.begin(), yawVect.end());
    data.insert(data.end(), thrustVect.begin(), thrustVect.end());

    CRTPPacket  packet(Port::Commander,Channel::TOC, std::move(data));

    return _crazyRadio.SendPacket_2(std::move(packet));
}

void Crazyflie::SetThrust(int thrust)
{
    _thrust = thrust;

    if(_thrust < _minThrust)
    {
        _thrust = _minThrust;
    }
    else if(_thrust > _maxThrust)
    {
        _thrust = _maxThrust;
    }
}

int Crazyflie::GetThrust()
{
    return GetSensorValue("stabilizer.thrust");
}

void Crazyflie::EnableStateMachine(bool enable)
{
    _stateMachineIsEnabled = enable;
}

bool Crazyflie::Update()
{
    // TODO SF How to restart the state machine properly?
    if(!_stateMachineIsEnabled)
    {
        _state = State::STATE_ZERO;
        return false;
    }

    switch(_state)
    {
    case State::STATE_ZERO:
    {
        _state = State::STATE_READ_PARAMETERS_TOC;
        break;
    }
    case State::STATE_READ_PARAMETERS_TOC:
    {
        if(ReadTOCParameters())
        {
            _state =State:: STATE_READ_LOGS_TOC;
        }
        break;
    }
    case State::STATE_READ_LOGS_TOC:
    {
        if(ReadTOCLogs())
        {
            _state = State::STATE_START_LOGGING;
        }
        break;
    }
    case State::STATE_START_LOGGING:
    {
        StartLogging();
        _state = State::STATE_ZERO_MEASUREMENTS;
        break;
    }
    case State::STATE_ZERO_MEASUREMENTS:
    {
        _tocLogs->ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        // NOTE(winkler): Here, we can do measurement zero'ing. This is
        // not done at the moment, though. Reason: No readings to zero at
        // the moment. This might change when altitude becomes available.

        _state = State::STATE_NORMAL_OPERATION;
        break;
    }
    case State::STATE_NORMAL_OPERATION:
    {
        // Shove over the sensor readings from the radio to the Logs TOC.
        _tocLogs->ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        if(_sendsSetpoints)
        {
            // Send the current set point based on the previous calculations
            SendSetpoint(_roll, _pitch, _yaw, _thrust);
        }
        else
        {
            // Send a dummy packet for keepalive
            _crazyRadio.SendPingPacket();
        }
        break;
    }
    default:
        break;
    } // end switch

    if(_crazyRadio.AckReceived())
    {
        _ackMissCounter = 0;
    }
    else
    {
        _ackMissCounter++;
    }

    return _crazyRadio.IsUsbConnectionOk();
}

bool Crazyflie::IsCopterConnected()
{
    return _ackMissCounter < _ackMissTolerance;
}

void Crazyflie::SetRoll(float roll)
{
    _roll = roll;

    if(std::fabs(_roll) > _maxAbsRoll)
    {
        _roll = copysign(_maxAbsRoll, _roll);
    }
}

float Crazyflie::GetRoll()
{
    return this->GetSensorValue("stabilizer.roll");
}

void Crazyflie::SetPitch(float pitch)
{
    _pitch = pitch;

    if(std::fabs(_pitch) > _maxAbsPitch)
    {
        _pitch = copysign(_maxAbsPitch, _pitch);
    }
}

float Crazyflie::GetPitch()
{
    return GetSensorValue("stabilizer.pitch");
}

void Crazyflie::SetYaw(float yaw)
{
    _yaw = yaw;

    if(std::fabs(_yaw) > _maxYaw){
        _yaw = copysign(_maxYaw, _yaw);
    }
}

float Crazyflie::GetYaw()
{
    return GetSensorValue("stabilizer.yaw");
}

bool Crazyflie::IsInitialized()
{
    return _state == State::STATE_NORMAL_OPERATION;
}

void Crazyflie::StartLogging()
{
    // Register the desired sensor readings
    EnableStabilizerLogging();
    EnableGyroscopeLogging();
    EnableAccelerometerLogging();
    EnableBatteryLogging();
    EnableMagnetometerLogging();
    EnableAltimeterLogging();
}

void Crazyflie::StopLogging()
{
    DisableStabilizerLogging();
    DisableGyroscopeLogging();
    DisableAccelerometerLogging();
    DisableBatteryLogging();
    DisableMagnetometerLogging();
    DisableAltimeterLogging();
}

void Crazyflie::SetSendSetpoints(bool bSendSetpoints)
{
    _sendsSetpoints = bSendSetpoints;
}

bool Crazyflie::IsSendingSetpoints()
{
    return _sendsSetpoints;
}

double Crazyflie::GetSensorValue(std::string strName)
{
    return _tocLogs->DoubleValue(strName);
}

void Crazyflie::DisableLogging()
{
    _tocLogs->UnregisterLoggingBlock("high-speed");
    _tocLogs->UnregisterLoggingBlock("low-speed");
}

void Crazyflie::EnableStabilizerLogging()
{
    _tocLogs->RegisterLoggingBlock("stabilizer", 1000);

    _tocLogs->StartLogging("stabilizer.roll", "stabilizer");
    _tocLogs->StartLogging("stabilizer.pitch", "stabilizer");
    _tocLogs->StartLogging("stabilizer.yaw", "stabilizer");
}

void Crazyflie::EnableGyroscopeLogging()
{
    _tocLogs->RegisterLoggingBlock("gyroscope", 1000);

    _tocLogs->StartLogging("gyro.x", "gyroscope");
    _tocLogs->StartLogging("gyro.y", "gyroscope");
    _tocLogs->StartLogging("gyro.z", "gyroscope");
}

float Crazyflie::GyroX()
{
    return this->GetSensorValue("gyro.x");
}

float Crazyflie::GyroY() {
    return this->GetSensorValue("gyro.y");
}

float Crazyflie::GyroZ() {
    return this->GetSensorValue("gyro.z");
}

void Crazyflie::EnableAccelerometerLogging()
{
    _tocLogs->RegisterLoggingBlock("accelerometer", 1000);

    _tocLogs->StartLogging("acc.x", "accelerometer");
    _tocLogs->StartLogging("acc.y", "accelerometer");
    _tocLogs->StartLogging("acc.z", "accelerometer");
    _tocLogs->StartLogging("acc.zw", "accelerometer");
}

float Crazyflie::AccX()
{
    return this->GetSensorValue("acc.x");
}

float Crazyflie::AccY()
{
    return this->GetSensorValue("acc.y");
}

float Crazyflie::AccZ()
{
    return this->GetSensorValue("acc.z");
}

float Crazyflie::AccZW()
{
    return this->GetSensorValue("acc.zw");
}

void Crazyflie::DisableStabilizerLogging()
{
    _tocLogs->UnregisterLoggingBlock("stabilizer");
}

void Crazyflie::DisableGyroscopeLogging()
{
    _tocLogs->UnregisterLoggingBlock("gyroscope");
}

void Crazyflie::DisableAccelerometerLogging()
{
    _tocLogs->UnregisterLoggingBlock("accelerometer");
}

void Crazyflie::EnableBatteryLogging()
{
    _tocLogs->RegisterLoggingBlock("battery", 1000);

    _tocLogs->StartLogging("pm.vbat", "battery");
    _tocLogs->StartLogging("pm.state", "battery");
}

double Crazyflie::GetBatteryLevel()
{
    return this->GetSensorValue("pm.vbat");
}

float Crazyflie::GetBatteryState()
{
    return this->GetSensorValue("pm.state");
}

void Crazyflie::DisableBatteryLogging()
{
    _tocLogs->UnregisterLoggingBlock("battery");
}

void Crazyflie::EnableMagnetometerLogging()
{
    _tocLogs->RegisterLoggingBlock("magnetometer", 1000);

    _tocLogs->StartLogging("mag.x", "magnetometer");
    _tocLogs->StartLogging("mag.y", "magnetometer");
    _tocLogs->StartLogging("mag.z", "magnetometer");
}
float Crazyflie::MagX()
{
    return this->GetSensorValue("mag.x");
}
float Crazyflie::MagY()
{
    return this->GetSensorValue("mag.y");
}
float Crazyflie::MagZ()
{
    return this->GetSensorValue("mag.z");
}
void Crazyflie::DisableMagnetometerLogging()
{
    _tocLogs->UnregisterLoggingBlock("magnetometer");
}

void Crazyflie::EnableAltimeterLogging()
{
    _tocLogs->RegisterLoggingBlock("altimeter", 1000);
    _tocLogs->StartLogging("alti.asl", "altimeter");
    _tocLogs->StartLogging("alti.aslLong", "altimeter");
    _tocLogs->StartLogging("alti.pressure", "altimeter");
    _tocLogs->StartLogging("alti.temperature", "altimeter");
}

float Crazyflie::Asl()
{
    return this->GetSensorValue("alti.asl");
}
float Crazyflie::AslLong()
{
    return this->GetSensorValue("alti.aslLong");
}
float Crazyflie::Pressure()
{
    return this->GetSensorValue("alti.pressure");
}
float Crazyflie::Temperature()
{
    return this->GetSensorValue("alti.temperature");
}

void Crazyflie::DisableAltimeterLogging()
{
    _tocLogs->UnregisterLoggingBlock("altimeter");
}
