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


Crazyflie::Crazyflie(CrazyRadio & crazyRadio) :
    _crazyRadio(crazyRadio),
    _ackMissTolerance(100),
    _ackMissCounter(0),
    _sendSetPoint(),
    _maxSetPoint({45.0,45.0,180.0,60000}),
    _minThrust(0),
    _isSendingSetpoints(false),
    _startConnecting(false),
    _state (State::ZERO),
    _tocParameters(_crazyRadio, Port::Parameters),
    _tocLogs(_crazyRadio, Port::Log),
    _leaveConnectingState(),
    _sensorValues()
{}

Crazyflie::~Crazyflie()
{
    StopLogging();
}

bool Crazyflie::ReadTOCParameters()
{
    if(_tocParameters.RequestMetaData() )
    {
        if(_tocParameters.RequestItems())
        {
            return true;
        }
    }

    return false;
}

bool Crazyflie::ReadTOCLogs()
{
    auto meta_ok = _tocLogs.RequestMetaData();
    if(meta_ok)
    {
        auto req = _tocLogs.RequestItems();
        if(req)
        {
            return true;
        }
    }

    return false;
}

bool Crazyflie::SendSetpoint(SetPoint setPoint)
{
    // In python client, this line implementes the x-mode
    auto roll = (setPoint.roll - setPoint.pitch) *0.707f;
    auto pitch = (setPoint.roll + setPoint.pitch) *0.707f;

    auto data = ConvertTouint8_tVect(roll);
    auto pitchVect = ConvertTouint8_tVect( -pitch); // Warning: Is negated here.
    auto yawVect = ConvertTouint8_tVect(setPoint.yaw);
    auto thrustVect = ConvertTouint8_tVect(setPoint.thrust);

    data.insert(data.end(), pitchVect.begin(), pitchVect.end());
    data.insert(data.end(), yawVect.begin(), yawVect.end());
    data.insert(data.end(), thrustVect.begin(), thrustVect.end());

    CRTPPacket  packet(Port::Commander,Channel::TOC, std::move(data));

    return _crazyRadio.SendPacketAndCheck(std::move(packet));
}

void Crazyflie::StartConnecting(bool enable)
{
    _startConnecting = enable;
}

// Runs on 10ms.
void Crazyflie::Update()
{
    // TODO SF How to restart the state machine properly?
//    if(!_stateMachineIsEnabled)
//    {
//        _state = State::STATE_ZERO;
//        StopLogging();
//        return false;
//    }

    switch(_state)
    {
    case State::ZERO:
    {
        if(_startConnecting)
        {
             _ackMissCounter = 0;
            _state = State::READ_PARAMETERS_TOC;
        }
        break;
    }
    case State::READ_PARAMETERS_TOC:
    {
        // TODO SF State machine is in busy wait in this state if the crazy flie is not turned on. This should not be.
        bool success = ReadTOCParameters();
        if(success)
        {
            _state =State:: READ_LOGS_TOC;
        }

        if(_crazyRadio.LastSendAndReceiveFailed())
        {
            _state = State::ZERO;
            _startConnecting = false;
        }
        break;
    }
    case State::READ_LOGS_TOC:
    {
        if(ReadTOCLogs())
        {
            _state = State::START_LOGGING;
        }
        break;
    }
    case State::START_LOGGING:
    {
        StartLogging();
        _state = State::ZERO_MEASUREMENTS;
        break;
    }
    case State::ZERO_MEASUREMENTS:
    {
        _tocLogs.ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        // NOTE(winkler): Here, we can do measurement zero'ing. This is
        // not done at the moment, though. Reason: No readings to zero at
        // the moment. This might change when altitude becomes available.

        _state = State::NORMAL_OPERATION;
        break;
    }
    case State::NORMAL_OPERATION:
    {
        // Shove over the sensor readings from the radio to the Logs TOC.
        _tocLogs.ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        if(_isSendingSetpoints)
        {
            // Send the current set point based on the previous calculations
            SendSetpoint(_sendSetPoint);
            _isSendingSetpoints = false;
        }
        else
        {
            // Send a ping packet for keepalive
            _crazyRadio.SendPingPacket();
        }

        if(_crazyRadio.AckReceived())
        {
            _ackMissCounter = 0;
        }
        else if(_ackMissCounter < _ackMissTolerance)
        {
            ++_ackMissCounter;
        }
        if( _ackMissCounter == _ackMissTolerance )
        {
            ++_ackMissCounter;
            _startConnecting = false;
            _state = State::ZERO;
        }

        break;
    }
    default:
        break;
    } // end switch

  //  if(_state != State::STATE_ZERO)
 /*   {
        if(_crazyRadio.AckReceived())
        {
            _ackMissCounter = 0;
        }
        else if(_ackMissCounter < _ackMissTolerance)
        {
            std::cout << _ackMissCounter << std::endl;
            ++_ackMissCounter;
        }*/
    //}
    UpateSensorValues();
//    if(_state!= State::STATE_ZERO)
//    {
//        std::cout << "state = " <<static_cast<int>(_state )<< std::endl;
//    }
    //    return _crazyRadio.IsUsbConnectionOk(); // TODO SF: For what is this needed?
}

void Crazyflie::UpateSensorValues()
{
    _sensorValues.stabilizer.roll = GetSensorValue("stabilizer.roll");
    _sensorValues.stabilizer.yaw= GetSensorValue("stabilizer.yaw");
    _sensorValues.stabilizer.pitch = GetSensorValue("stabilizer.pitch");
    _sensorValues.stabilizer.thrust = GetSensorValue("stabilizer.thrust");
    _sensorValues.barometer.pressure = GetSensorValue("baro.pressure");
    _sensorValues.barometer.asl = GetSensorValue("baro.asl");
    _sensorValues.barometer.aslLong= GetSensorValue("baro.aslLong");
    _sensorValues.barometer.temperature = GetSensorValue("baro.temperature");
    _sensorValues.acceleration.x = GetSensorValue("acc.x");
    _sensorValues.acceleration.y = GetSensorValue("acc.y");
    _sensorValues.acceleration.z = GetSensorValue("acc.z");
    _sensorValues.acceleration.zw = GetSensorValue("acc.zw");
    _sensorValues.battery.level = GetSensorValue("pm.vbat");
    _sensorValues.battery.state = GetSensorValue("pm.state");
    _sensorValues.gyrometer.x = GetSensorValue("gyro.x");
    _sensorValues.gyrometer.y = GetSensorValue("gyro.y");
    _sensorValues.gyrometer.z = GetSensorValue("gyro.z");
    _sensorValues.magnetometer.x = GetSensorValue("mag.x");
    _sensorValues.magnetometer.y = GetSensorValue("mag.y");
    _sensorValues.magnetometer.z = GetSensorValue("mag.z");
}

bool Crazyflie::IsConnectionTimeout()
{
    return _ackMissCounter == _ackMissTolerance+1;
}


void Crazyflie::SetSetPoint(SetPoint setPoint)
{
    SetThrust(setPoint.thrust);
    SetYaw(setPoint.yaw);
    SetRoll(setPoint.roll);
    SetPitch(setPoint.pitch);
}


// TODO SF: Simplifly setpoint setting
void Crazyflie::SetThrust(int thrust)
{
    _sendSetPoint.thrust = thrust;

    if(_sendSetPoint.thrust < _minThrust)
    {
        _sendSetPoint.thrust = _minThrust;
    }
    else if(_sendSetPoint.thrust > _sendSetPoint.thrust)
    {
        _sendSetPoint.thrust = _sendSetPoint.thrust;
    }
}
void Crazyflie::SetRoll(float roll)
{
    _sendSetPoint.roll = roll;

    if(std::fabs(_sendSetPoint.roll) > _sendSetPoint.roll)
    {
        _sendSetPoint.roll = copysign(_sendSetPoint.roll, _sendSetPoint.roll);
    }
}

void Crazyflie::SetPitch(float pitch)
{
    _sendSetPoint.pitch = pitch;

    if(std::fabs(_sendSetPoint.pitch) > _sendSetPoint.pitch)
    {
        _sendSetPoint.pitch = copysign(_sendSetPoint.pitch, _sendSetPoint.pitch);
    }
}

void Crazyflie::SetYaw(float yaw)
{
    _sendSetPoint.yaw = yaw;

    if(std::fabs(_sendSetPoint.yaw) > _sendSetPoint.yaw)
    {
        _sendSetPoint.yaw = copysign(_sendSetPoint.yaw, _sendSetPoint.yaw);
    }
}

bool Crazyflie::IsDisconnected()
{
    return _state == State::ZERO;
}
bool Crazyflie::IsConnecting()
{
    return !(IsDisconnected() || IsConnected());
}
bool Crazyflie::IsConnected()
{
    return _state == State::NORMAL_OPERATION;
}

void Crazyflie::StartLogging()
{
    // Register the desired sensor readings
    EnableStabilizerLogging();
    EnableGyroscopeLogging();
    EnableAccelerometerLogging();
    EnableBatteryLogging();
    EnableMagnetometerLogging();
    EnableBarometerLogging();
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

void Crazyflie::DisableLogging()
{
    _tocLogs.UnregisterLoggingBlock("high-speed");
    _tocLogs.UnregisterLoggingBlock("low-speed");
}

void Crazyflie::SetSendSetpoints(bool sendSetpoints)
{
    _isSendingSetpoints = sendSetpoints;
}

bool Crazyflie::IsSendingSetpoints()
{
    return _isSendingSetpoints;
}

double Crazyflie::GetSensorValue(std::string strName)
{
    return _tocLogs.DoubleValue(strName);
}


void Crazyflie::EnableStabilizerLogging()
{
    _tocLogs.RegisterLoggingBlock("stabilizer", _frequency);

    _tocLogs.StartLogging("stabilizer.roll", "stabilizer");
    _tocLogs.StartLogging("stabilizer.pitch", "stabilizer");
    _tocLogs.StartLogging("stabilizer.yaw", "stabilizer");
    _tocLogs.StartLogging("stabilizer.thrust", "stabilizer");
}

void Crazyflie::EnableGyroscopeLogging()
{
    _tocLogs.RegisterLoggingBlock("gyroscope", _frequency);

    _tocLogs.StartLogging("gyro.x", "gyroscope");
    _tocLogs.StartLogging("gyro.y", "gyroscope");
    _tocLogs.StartLogging("gyro.z", "gyroscope");
}

void Crazyflie::EnableAccelerometerLogging()
{
    _tocLogs.RegisterLoggingBlock("accelerometer", _frequency);

    _tocLogs.StartLogging("acc.x", "accelerometer");
    _tocLogs.StartLogging("acc.y", "accelerometer");
    _tocLogs.StartLogging("acc.z", "accelerometer");
    _tocLogs.StartLogging("acc.zw", "accelerometer");
}


void Crazyflie::DisableStabilizerLogging()
{
    _tocLogs.UnregisterLoggingBlock("stabilizer");
}

void Crazyflie::DisableGyroscopeLogging()
{
    _tocLogs.UnregisterLoggingBlock("gyroscope");
}

void Crazyflie::DisableAccelerometerLogging()
{
    _tocLogs.UnregisterLoggingBlock("accelerometer");
}

void Crazyflie::EnableBatteryLogging()
{
    _tocLogs.RegisterLoggingBlock("battery", _frequency);

    _tocLogs.StartLogging("pm.vbat", "battery");
    _tocLogs.StartLogging("pm.state", "battery");
}

void Crazyflie::DisableBatteryLogging()
{
    _tocLogs.UnregisterLoggingBlock("battery");
}

void Crazyflie::EnableMagnetometerLogging()
{
    _tocLogs.RegisterLoggingBlock("magnetometer", _frequency);

    _tocLogs.StartLogging("mag.x", "magnetometer");
    _tocLogs.StartLogging("mag.y", "magnetometer");
    _tocLogs.StartLogging("mag.z", "magnetometer");
}

void Crazyflie::DisableMagnetometerLogging()
{
    _tocLogs.UnregisterLoggingBlock("magnetometer");
}

void Crazyflie::EnableBarometerLogging()
{
    _tocLogs.RegisterLoggingBlock("barometer", _frequency);
    _tocLogs.StartLogging("baro.asl", "altimeter");
    _tocLogs.StartLogging("baro.aslLong", "altimeter");
    _tocLogs.StartLogging("baro.pressure", "altimeter");
    _tocLogs.StartLogging("baro.temperature", "altimeter");
}

void Crazyflie::DisableAltimeterLogging()
{
    _tocLogs.UnregisterLoggingBlock("barometer");
}


