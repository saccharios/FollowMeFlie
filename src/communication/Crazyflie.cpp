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
#include "math/constants.h"
#include "math/types.h"

Crazyflie::Crazyflie(CrazyRadio & crazyRadio) :
    _crazyRadio(crazyRadio),
    _ackMissTolerance(100),
    _ackMissCounter(0),
    _sendSetPoint(),
    _maxSetPoint({45.0,45.0,180.0,60000}),
    _minThrust(0),
    _isSendingSetpoints(false),
    _isSendingVelocityRef(false),
    _startConnecting(false),
    _state (State::ZERO),
    _parameters(_crazyRadio),
    _logger(_crazyRadio),
    _leaveConnectingState(),
    _sensorValues()
{}

Crazyflie::~Crazyflie()
{
    StopLogging();
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


    //    std::cout << "State = " << static_cast<int>(_state) << std::endl;

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
        if( !_crazyRadio.IsUsbConnectionOk())
        {
            _state = State::ZERO;
            _startConnecting = false;
            emit NotConnecting();

        }
        else
        {
            // Setup Parameter TOC
            bool success = SetupParameters();
            if(success)
            {
                _state =State:: READ_LOGS_TOC;
            }

            if(_crazyRadio.LastSendAndReceiveFailed())
            {
                _state = State::ZERO;
                _startConnecting = false;
                emit NotConnecting();
            }
        }
        break;
    }
    case State::READ_LOGS_TOC:
    {
        // Setup Logging TOC
        if(SetupLogger())
        {
            StartLogging();
            _state = State::ZERO_MEASUREMENTS;
        }
        break;
    }
    case State::ZERO_MEASUREMENTS:
    {
        //        _tocLogs.ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        _logger.ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        _crazyRadio.SendPingPacket();
        // NOTE(winkler): Here, we can do measurement zero'ing. This is
        // not done at the moment, though. Reason: No readings to zero at
        // the moment. This might change when altitude becomes available.


        _setPointOffset.roll = GetSensorValue("stabilizer.roll");
        _setPointOffset.yaw= 0.0f;
        _setPointOffset.pitch = GetSensorValue("stabilizer.pitch");
        _setPointOffset.thrust = GetSensorValue("stabilizer.thrust");
        _accelerationOffset[0] = GetSensorValue("acc.x");
        _accelerationOffset[1] = GetSensorValue("acc.y");
        _accelerationOffset[2] = GetSensorValue("acc.z");

        //        _setPointOffset.Print();
        //        std::cout << "acc_x_offset = " << _accelerationOffset[0] << " acc_y_offset = " << _accelerationOffset[1] << " acc_z_offset = " << _accelerationOffset[2] << std::endl;

        // In the first six queries the acceleration value is still zero
        static int counter = 0;
        if(counter > 7)
        {
            _state = State::NORMAL_OPERATION;
        }
        ++counter;
        break;
    }
    case State::NORMAL_OPERATION:
    {
        // Shove over the sensor readings from the radio to the Logs TOC.
        //        _tocLogs.ProcessLogPackets(_crazyRadio.PopLoggingPackets());
        _logger.ProcessLogPackets(_crazyRadio.PopLoggingPackets());

        if(_isSendingSetpoints)
        {
            // Send the current set point based on the previous calculations
            SendSetpoint(_sendSetPoint);
            _isSendingSetpoints = false;
        }
        else if(_isSendingVelocityRef)
        {
            SendVelocityRef(_velocity);
            _isSendingVelocityRef = false;
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
        else
        {
            _startConnecting = false;
            emit ConnectionTimeout();
            _state = State::ZERO;
        }

        break;
    }
    default:
        break;
    } // end switch

    UpateSensorValues();
}
bool Crazyflie::SetupParameters()
{
    // This function is called periodically. It may be that it takes more than one sampling period to
    // execute it. Use this bool guards to prevent a deadlock on the crazyflie.
    static bool is_running = false;
    if(!is_running)
    {
        is_running = true;
        bool  info_ok = _parameters.RequestInfo();
        if(info_ok)
        {
            bool items_ok = _parameters.RequestItems();
            if(items_ok)
            {
                is_running = false;
                return true;
            }
            else
            {
                std::cout << "Parameter TOC: Failed to get items\n";
                is_running = false;
                return false;
            }
        }
        else
        {
            std::cout << "Parameter TOC: Failed to get info\n";
            is_running = false;
            return false;
        }
    }
    return false;
}


bool Crazyflie::SetupLogger()
{

    // This function is called periodically. It may be that it takes more than one sampling period to
    // execute it. Use this bool guards to prevent a deadlock on the crazyflie.
    static bool is_running = false;
    if(!is_running)
    {
        is_running = true;
        bool  info_ok = _logger.RequestInfo();
        if(info_ok)
        {
            bool items_ok = _logger.RequestItems();
            if(items_ok)
            {
                is_running = false;
                return true;
            }
            else
            {
                std::cout << "Logger TOC: Failed to get items\n";
                is_running = false;
                return false;
            }
        }
        else
        {
            std::cout << "Logger TOC: Failed to get info\n";
            is_running = false;
            return false;
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

bool  Crazyflie::SendVelocityRef(Velocity velocity)
{
    // TODO SF  also x -mode?
    Data data;
    uint8_t inidicator = 1;
    auto vx_vect = ConvertTouint8_tVect(velocity[0]);
    auto vy_vect = ConvertTouint8_tVect(velocity[1]);
    auto vz_vect = ConvertTouint8_tVect(velocity[2]);
    auto yaw_vect = ConvertTouint8_tVect(0.0f);
    data.push_back(inidicator);
    data.insert(data.end(), vx_vect.begin(), vx_vect.end());
    data.insert(data.end(), vy_vect.begin(), vy_vect.end());
    data.insert(data.end(), vz_vect.begin(), vz_vect.end());
    data.insert(data.end(), yaw_vect.begin(), yaw_vect.end());

    CRTPPacket  packet(Port::Commander_Generic,Channel::TOC, std::move(data));

    return _crazyRadio.SendPacketAndCheck(std::move(packet));
}



void Crazyflie::StartConnecting(bool enable)
{
    _startConnecting = enable;
}



void Crazyflie::UpateSensorValues()
{
    _sensorValues.stabilizer.roll = GetSensorValue("stabilizer.roll") - _setPointOffset.roll;
    _sensorValues.stabilizer.yaw= GetSensorValue("stabilizer.yaw") - _setPointOffset.yaw;
    _sensorValues.stabilizer.pitch = GetSensorValue("stabilizer.pitch") - _setPointOffset.pitch;
    _sensorValues.stabilizer.thrust = GetSensorValue("stabilizer.thrust") - _setPointOffset.thrust;
    _sensorValues.barometer.pressure = GetSensorValue("baro.pressure");
    _sensorValues.barometer.asl = GetSensorValue("baro.asl");
    _sensorValues.barometer.aslLong= GetSensorValue("baro.aslLong");
    _sensorValues.barometer.temperature = GetSensorValue("baro.temperature");
    _sensorValues.acceleration.x = GetSensorValue("acc.x") - _accelerationOffset[0];
    _sensorValues.acceleration.y = GetSensorValue("acc.y") - _accelerationOffset[1];
    _sensorValues.acceleration.z = GetSensorValue("acc.z") - _accelerationOffset[2];
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

void Crazyflie::SetSetPoint(SetPoint setPoint)
{
    SetThrust(setPoint.thrust);
    SetYaw(setPoint.yaw);
    SetRoll(setPoint.roll);
    SetPitch(setPoint.pitch);
}


void Crazyflie::SetVelocityRef(Velocity velocity)
{
    _velocity = velocity;
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
    _logger.UnregisterLoggingBlock("high-speed");
    _logger.UnregisterLoggingBlock("low-speed");
}

void Crazyflie::SetSendSetpoints(bool sendSetpoints)
{
    _isSendingSetpoints = sendSetpoints;
}
void Crazyflie::SetSendingVelocityRef(bool isSendingVelocityRef)
{
    _isSendingVelocityRef = isSendingVelocityRef;
}

bool Crazyflie::IsSendingSetpoints()
{
    return _isSendingSetpoints;
}
bool Crazyflie::IsSendingVelocityRef()
{
    return _isSendingVelocityRef;
}

float Crazyflie::GetSensorValue(std::string strName)
{
    return _logger.Value(strName);
}


void Crazyflie::EnableStabilizerLogging()
{
    _logger.RegisterLoggingBlock("stabilizer", _frequency);
    _logger.StartLogging("stabilizer.roll", "stabilizer");
    _logger.StartLogging("stabilizer.pitch", "stabilizer");
    _logger.StartLogging("stabilizer.yaw", "stabilizer");
    _logger.StartLogging("stabilizer.thrust", "stabilizer");
}

void Crazyflie::EnableGyroscopeLogging()
{
    _logger.RegisterLoggingBlock("gyroscope", _frequency);
    _logger.StartLogging("gyro.x", "gyroscope");
    _logger.StartLogging("gyro.y", "gyroscope");
    _logger.StartLogging("gyro.z", "gyroscope");
}

void Crazyflie::EnableAccelerometerLogging()
{
    _logger.RegisterLoggingBlock("accelerometer", _frequency);
    _logger.StartLogging("acc.x", "accelerometer");
    _logger.StartLogging("acc.y", "accelerometer");
    _logger.StartLogging("acc.z", "accelerometer");
    _logger.StartLogging("acc.zw", "accelerometer");
}


void Crazyflie::DisableStabilizerLogging()
{
    _logger.UnregisterLoggingBlock("stabilizer");
}

void Crazyflie::DisableGyroscopeLogging()
{
    _logger.UnregisterLoggingBlock("gyroscope");
}

void Crazyflie::DisableAccelerometerLogging()
{
    _logger.UnregisterLoggingBlock("accelerometer");
}

void Crazyflie::EnableBatteryLogging()
{
    _logger.RegisterLoggingBlock("battery", _frequency);
    _logger.StartLogging("pm.vbat", "battery");
    _logger.StartLogging("pm.state", "battery");
}

void Crazyflie::DisableBatteryLogging()
{
    _logger.UnregisterLoggingBlock("battery");
}

void Crazyflie::EnableMagnetometerLogging()
{
    _logger.RegisterLoggingBlock("magnetometer", _frequency);
    _logger.StartLogging("mag.x", "magnetometer");
    _logger.StartLogging("mag.y", "magnetometer");
    _logger.StartLogging("mag.z", "magnetometer");
}

void Crazyflie::DisableMagnetometerLogging()
{
    _logger.UnregisterLoggingBlock("magnetometer");
}

void Crazyflie::EnableBarometerLogging()
{
    _logger.RegisterLoggingBlock("barometer", _frequency);
    _logger.StartLogging("baro.asl", "altimeter");
    _logger.StartLogging("baro.aslLong", "altimeter");
    _logger.StartLogging("baro.pressure", "altimeter");
    _logger.StartLogging("baro.temperature", "altimeter");
}

void Crazyflie::DisableAltimeterLogging()
{
    _logger.UnregisterLoggingBlock("barometer");
}


std::array<float,3> Crazyflie::ConvertBodyFrameToIntertialFrame(std::array<float,3> const & value_in_body)
{

    auto const & sensorValues = GetSensorValues();

    auto sin_roll = sinf(sensorValues.stabilizer.roll/180.0f*pi);
    auto cos_roll = cosf(sensorValues.stabilizer.roll/180.0f*pi);
    auto sin_pitch = sinf(sensorValues.stabilizer.pitch/180.0f*pi);
    auto cos_pitch = cosf(sensorValues.stabilizer.pitch/180.0f*pi);
    auto sin_yaw = sinf(sensorValues.stabilizer.yaw/180.0f*pi);
    auto cos_yaw = cosf(sensorValues.stabilizer.yaw/180.0f*pi);


    auto x_b = -value_in_body[0]; // X-Axis is in negative direction (SED not NED on drone)
    auto y_b = value_in_body[1];
    auto z_b = value_in_body[2];
    std::array<float,3> value_in_inertial;
    value_in_inertial[0] = cos_yaw * cos_pitch*x_b + (cos_yaw * sin_pitch * sin_roll -  sin_yaw  * cos_roll)*y_b + (cos_yaw*sin_pitch*cos_roll + sin_yaw*sin_roll)* z_b;
    value_in_inertial[1] = sin_yaw  * cos_pitch*x_b + (sin_yaw  * sin_pitch * sin_roll + cos_yaw * cos_roll)*y_b + (sin_yaw*sin_pitch*cos_roll  - cos_yaw*sin_roll)* z_b;
    value_in_inertial[2]  = -sin_pitch  * x_b + cos_pitch * sin_roll * y_b + cos_pitch * cos_roll*z_b;
    return value_in_inertial;
}

