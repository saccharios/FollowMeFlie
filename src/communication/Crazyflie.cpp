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
#include "protocol.h"
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


//        std::cout << "State = " << static_cast<int>(_state) << std::endl;

    switch(_state)
    {
    case State::ZERO:
    {
        if(_startConnecting)
        {
            _ackMissCounter = 0;
            _state = State::SETUP_PARAMETERS;
        }
        break;
    }
    case State::SETUP_PARAMETERS:
    {
        if( !_crazyRadio.IsUsbConnectionOk())
        {
            _state = State::ZERO;
            _startConnecting = false;
            emit NotConnecting();

        }
        else
        {
            bool success = _parameters.Setup();
            if(success)
            {
                _state =State::READ_PARAMETERS;
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
    case State::READ_PARAMETERS:
    {
        bool success = _parameters.ReadAll();
        if(success)
        {
            _state =State::SETUP_LOGGER;
        }
        break;
    }
    case State::SETUP_LOGGER:
    {
        bool success = _logger.Setup();
        if(success)
        {
            bool success2 = StartLogging();
            if(success2)
            {
                _state = State::ZERO_MEASUREMENTS;
            }
        }
        break;
    }
    case State::ZERO_MEASUREMENTS:
    {
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

bool Crazyflie::SendSetpoint(SetPoint setPoint)
{
    // In python client, this line implementes the x-mode
    auto roll = (setPoint.roll - setPoint.pitch) *SQRT2;
    auto pitch = (setPoint.roll + setPoint.pitch) *SQRT2;

    auto data = ConvertTouint8_tVect(roll);
    auto pitchVect = ConvertTouint8_tVect( -pitch); // Warning: Is negated here.
    auto yawVect = ConvertTouint8_tVect(setPoint.yaw);
    auto thrustVect = ConvertTouint8_tVect(setPoint.thrust);

    data.insert(data.end(), pitchVect.begin(), pitchVect.end());
    data.insert(data.end(), yawVect.begin(), yawVect.end());
    data.insert(data.end(), thrustVect.begin(), thrustVect.end());

    CRTPPacket  packet(Port::Commander, Commander_Channels::Setpoint::id, std::move(data));

    return _crazyRadio.SendPacketAndCheck(std::move(packet));
}

bool  Crazyflie::SendVelocityRef(Velocity velocity)
{
    // TODO SF  also x -mode?
    Data data;
    uint8_t command = 1;
    auto vx_vect = ConvertTouint8_tVect(velocity[0]);
    auto vy_vect = ConvertTouint8_tVect(velocity[1]);
    auto vz_vect = ConvertTouint8_tVect(velocity[2]);
    auto yaw_vect = ConvertTouint8_tVect(0.0f);
    data.push_back(command);
    data.insert(data.end(), vx_vect.begin(), vx_vect.end());
    data.insert(data.end(), vy_vect.begin(), vy_vect.end());
    data.insert(data.end(), vz_vect.begin(), vz_vect.end());
    data.insert(data.end(), yaw_vect.begin(), yaw_vect.end());

    CRTPPacket  packet(Port::Commander_Generic, CommanderGeneric_Channels::GenericSetpoint::id, std::move(data));

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

bool Crazyflie::StartLogging()
{
        // Register the desired sensor readings
        bool success = RegisterLoggingBlocks();
        if(success )
        {
            EnableSensorsLogging();
            EnableBatteryLogging();
            EnablePIDAttitudeLogging();
            EnablePIDRateLogging();
            EnableControllerLogging();
            EnableKalman1Logging();
            EnableKalman2Logging();
            EnablePosCtrlLogging();
            EnableAltitudeEstimationLogging();
            EnableMotorsLogging();
            EnableSensorFusionLogging();
            EnableCtrlTargetLogging();
            EnableStateEstimateLogging();
            return true;
        }
    return false;
}

void Crazyflie::StopLogging()
{
    DisableSensorsLogging();
    DisableBatteryLogging();
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

bool Crazyflie::RegisterLoggingBlocks()
{
    static bool is_running = false;
    constexpr int max_try = 3;
    static int cntr = 0;
    if(!is_running)
    {
        is_running = true;
        // Repeat as long as not all logging blocks are registered, at max max_try;
        static constexpr int num_loggers = 13;
        static std::array<bool,num_loggers> success ={false,false,false,false,false,false,
                                           false,false,false,false,false,false,
                                           false};
        // TODO SF:: Logging Blocks should be classes
        RegisterLogginBlock(success[0], "sensors", _frequency);
        RegisterLogginBlock(success[1], "battery", _frequency/8.0f);
        RegisterLogginBlock(success[2], "pid_attitude", _frequency);
        RegisterLogginBlock(success[3], "pid_rate", _frequency);
        RegisterLogginBlock(success[4], "controller", _frequency);
        RegisterLogginBlock(success[5], "kalman_1", _frequency/2.0f);
        RegisterLogginBlock(success[6], "kalman_2", _frequency/2.0f);
        RegisterLogginBlock(success[7], "position_ctrl", _frequency/2.0f);
        RegisterLogginBlock(success[8], "alt_est", _frequency/2.0f);
        RegisterLogginBlock(success[9], "motors", _frequency/2.0f);
        RegisterLogginBlock(success[10], "sensor_fusion", _frequency/2.0f);
        RegisterLogginBlock(success[11], "ctrl_target", _frequency/2.0f);
        RegisterLogginBlock(success[12], "state_estimate", _frequency/2.0f);
        bool total = true;
        for(int i = 0; i < num_loggers; ++i)
        {
            total = (total && success[i]);
        }

        if ( total || (cntr > max_try) )
        {
            is_running = false;
            return true;
        }
        else
        {
            ++cntr;
            is_running = false;
            return false;
        }
    }
    return false;
}

void Crazyflie::RegisterLogginBlock(bool & success, std::string name, float frequency)
{
    if(!success)
    {
        success = _logger.RegisterLoggingBlock(name, frequency);
    }
}

void Crazyflie::EnableSensorsLogging()
{
    _logger.StartLogging("stabilizer.roll", "sensors");
    _logger.StartLogging("stabilizer.pitch", "sensors");
    _logger.StartLogging("stabilizer.yaw", "sensors");
    _logger.StartLogging("stabilizer.thrust", "sensors");
    _logger.StartLogging("gyro.x", "sensors");
    _logger.StartLogging("gyro.y", "sensors");
    _logger.StartLogging("gyro.z", "sensors");
    _logger.StartLogging("acc.x", "sensors");
    _logger.StartLogging("acc.y", "sensors");
    _logger.StartLogging("acc.z", "sensors");
    _logger.StartLogging("acc.zw", "sensors");
    _logger.StartLogging("mag.x", "sensors");
    _logger.StartLogging("mag.y", "sensors");
    _logger.StartLogging("mag.z", "sensors");
    _logger.StartLogging("baro.asl", "sensors");
    _logger.StartLogging("baro.aslLong", "sensors");
    _logger.StartLogging("baro.pressure", "sensors");
    _logger.StartLogging("baro.temperature", "sensors");
}

void Crazyflie::DisableSensorsLogging()
{
    _logger.UnregisterLoggingBlock("sensors");
}

void Crazyflie::EnableBatteryLogging()
{
    _logger.StartLogging("pm.vbat", "battery");
    _logger.StartLogging("pm.state", "battery");
    _logger.StartLogging("radio.rssi", "battery");
}

void Crazyflie::DisableBatteryLogging()
{
    _logger.UnregisterLoggingBlock("battery");
}

void Crazyflie::EnablePIDAttitudeLogging()
{
    _logger.StartLogging("pid_attitude.roll_outP", "pid_attitude");
    _logger.StartLogging("pid_attitude.roll_outI", "pid_attitude");
    _logger.StartLogging("pid_attitude.roll_outD", "pid_attitude");
    _logger.StartLogging("pid_attitude.pitch_outP", "pid_attitude");
    _logger.StartLogging("pid_attitude.pitch_outI", "pid_attitude");
    _logger.StartLogging("pid_attitude.pitch_outD", "pid_attitude");
    _logger.StartLogging("pid_attitude.yaw_outP", "pid_attitude");
    _logger.StartLogging("pid_attitude.yaw_outI", "pid_attitude");
    _logger.StartLogging("pid_attitude.yaw_outD", "pid_attitude");
}

void Crazyflie::DisablePIDAttitudeLogging()
{
    _logger.UnregisterLoggingBlock("pid_attitude");
}
void Crazyflie::EnablePIDRateLogging()
{
    _logger.StartLogging("pid_rate.roll_outP", "pid_rate");
    _logger.StartLogging("pid_rate.roll_outI", "pid_rate");
    _logger.StartLogging("pid_rate.roll_outD", "pid_rate");
    _logger.StartLogging("pid_rate.pitch_outP", "pid_rate");
    _logger.StartLogging("pid_rate.pitch_outI", "pid_rate");
    _logger.StartLogging("pid_rate.pitch_outD", "pid_rate");
    _logger.StartLogging("pid_rate.yaw_outP", "pid_rate");
    _logger.StartLogging("pid_rate.yaw_outI", "pid_rate");
    _logger.StartLogging("pid_rate.yaw_outD", "pid_rate");
}

void Crazyflie::DisablePIDRateLogging()
{
    _logger.UnregisterLoggingBlock("pid_rate");
}

void Crazyflie::EnableControllerLogging()
{
    _logger.StartLogging("controller.actuatorThrust", "controller");
    _logger.StartLogging("controller.rol", "controller");
    _logger.StartLogging("controller.pitch", "controller");
    _logger.StartLogging("controller.yaw", "controller");
    _logger.StartLogging("controller.rollRate", "controller");
    _logger.StartLogging("controller.pitchRate", "controller");
    _logger.StartLogging("controller.yawRate", "controller");
    _logger.StartLogging("controller.ctr_yaw", "controller");
}

void Crazyflie::DisableControllerLogging()
{
    _logger.UnregisterLoggingBlock("controller");
}
void Crazyflie::EnableKalman1Logging()
{
    _logger.StartLogging("kalman_states.ox", "kalman_1");
    _logger.StartLogging("kalman_states.oy", "kalman_1");
    _logger.StartLogging("kalman_states.vx", "kalman_1");
    _logger.StartLogging("kalman_states.vy", "kalman_1");
    _logger.StartLogging("kalman_pred.predNX", "kalman_1");
    _logger.StartLogging("kalman_pred.predNY", "kalman_1");
    _logger.StartLogging("kalman_pred.measNX", "kalman_1");
    _logger.StartLogging("kalman_pred.measNY", "kalman_1");
    _logger.StartLogging("kalman.inFlight", "kalman_1");
    _logger.StartLogging("kalman.stateX", "kalman_1");
    _logger.StartLogging("kalman.stateY", "kalman_1");
    _logger.StartLogging("kalman.stateZ", "kalman_1");
    _logger.StartLogging("kalman.statePX", "kalman_1");
    _logger.StartLogging("kalman.statePY", "kalman_1");
    _logger.StartLogging("kalman.statePZ", "kalman_1");
    _logger.StartLogging("kalman.stateD0", "kalman_1");
    _logger.StartLogging("kalman.stateD1", "kalman_1");
    _logger.StartLogging("kalman.stateD2", "kalman_1");
    _logger.StartLogging("kalman.stateSkew", "kalman_1");
}

void Crazyflie::DisableKalman1Logging()
{
    _logger.UnregisterLoggingBlock("kalman_1");
}

void Crazyflie::EnableKalman2Logging()
{
        _logger.StartLogging("kalman.varX", "kalman_2");
        _logger.StartLogging("kalman.varY", "kalman_2");
        _logger.StartLogging("kalman.varZ", "kalman_2");
        _logger.StartLogging("kalman.varPX", "kalman_2");
        _logger.StartLogging("kalman.varPY", "kalman_2");
        _logger.StartLogging("kalman.varPZ", "kalman_2");
        _logger.StartLogging("kalman.varD0", "kalman_2");
        _logger.StartLogging("kalman.varD1", "kalman_2");
        _logger.StartLogging("kalman.varD2", "kalman_2");
        _logger.StartLogging("kalman.varD3", "kalman_2");
        _logger.StartLogging("kalman.varSkew", "kalman_2");
        _logger.StartLogging("kalman.q0", "kalman_2");
        _logger.StartLogging("kalman.q1", "kalman_2");
        _logger.StartLogging("kalman.q2", "kalman_2");
        _logger.StartLogging("kalman.q3", "kalman_2");
}

void Crazyflie::DisableKalman2Logging()
{
    _logger.UnregisterLoggingBlock("kalman_2");
}

void Crazyflie::EnablePosCtrlLogging()
{
    _logger.StartLogging("posCtl.targetVX", "position_ctrl");
    _logger.StartLogging("posCtl.targetVY", "position_ctrl");
    _logger.StartLogging("posCtl.targetVZ", "position_ctrl");
    _logger.StartLogging("posCtl.targetX", "position_ctrl");
    _logger.StartLogging("posCtl.targetY", "position_ctrl");
    _logger.StartLogging("posCtl.targetZ", "position_ctrl");
    _logger.StartLogging("posCtl.Xp", "position_ctrl");
    _logger.StartLogging("posCtl.Xi", "position_ctrl");
    _logger.StartLogging("posCtl.Xd", "position_ctrl");
    _logger.StartLogging("posCtl.Yp", "position_ctrl");
    _logger.StartLogging("posCtl.Yi", "position_ctrl");
    _logger.StartLogging("posCtl.Yd", "position_ctrl");
    _logger.StartLogging("posCtl.Zp", "position_ctrl");
    _logger.StartLogging("posCtl.Zi", "position_ctrl");
    _logger.StartLogging("posCtl.Zd", "position_ctrl");
    _logger.StartLogging("posCtl.VXp", "position_ctrl");
    _logger.StartLogging("posCtl.VXi", "position_ctrl");
    _logger.StartLogging("posCtl.VXd", "position_ctrl");
    _logger.StartLogging("posCtl.VZp", "position_ctrl");
    _logger.StartLogging("posCtl.VZi", "position_ctrl");
    _logger.StartLogging("posCtl.VZd", "position_ctrl");
}

void Crazyflie::DisablePosCtrlLogging()
{
    _logger.UnregisterLoggingBlock("position_ctrl");
}
void Crazyflie::EnableAltitudeEstimationLogging()
{
    _logger.StartLogging("posEstimatorAlt.estimatedZ", "alt_est");
    _logger.StartLogging("posEstimatorAlt.estVZ", "alt_est");
    _logger.StartLogging("posEstimatorAlt.velocityZ", "alt_est");
}

void Crazyflie::DisableAltitudeEstimationLogging()
{
    _logger.UnregisterLoggingBlock("alt_est");
}
void Crazyflie::EnableMotorsLogging()
{
    _logger.StartLogging("motor.m1", "motors");
    _logger.StartLogging("motor.m2", "motors");
    _logger.StartLogging("motor.m3", "motors");
    _logger.StartLogging("motor.m4", "motors");
    _logger.StartLogging("pwm.m1_pwm", "motors");
    _logger.StartLogging("pwm.m2_pwm", "motors");
    _logger.StartLogging("pwm.m3_pwm", "motors");
    _logger.StartLogging("pwm.m4_pwm", "motors");
}

void Crazyflie::DisableMotorsLogging()
{
    _logger.UnregisterLoggingBlock("motors");
}
void Crazyflie::EnableSensorFusionLogging()
{
    _logger.StartLogging("sensorfusion6.qw", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.qx", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.qy", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.qz", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.gravityX", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.gravityY", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.gravityZ", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.accZbase", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.isInit", "sensor_fusion");
    _logger.StartLogging("sensorfusion6.isCalibrated", "sensor_fusion");
}

void Crazyflie::DisableSensorFusionLogging()
{
    _logger.UnregisterLoggingBlock("sensor_fusion");
}
void Crazyflie::EnableCtrlTargetLogging()
{
    _logger.StartLogging("ctrltarget.roll", "ctrl_target");
    _logger.StartLogging("ctrltarget.pitch", "ctrl_target");
    _logger.StartLogging("ctrltarget.yaw", "ctrl_target");
}

void Crazyflie::DisableCtrlTargetLogging()
{
    _logger.UnregisterLoggingBlock("ctrl_target");
}
void Crazyflie::EnableStateEstimateLogging()
{
    _logger.StartLogging("stateEstimate.x", "state_estimate");
    _logger.StartLogging("stateEstimate.y", "state_estimate");
    _logger.StartLogging("stateEstimate.z", "state_estimate");
}

void Crazyflie::DisableStateEstimateLogging()
{
    _logger.UnregisterLoggingBlock("stateEstimate");
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

