#include "crazy_flie.h"
#include <chrono>
#include "math/constants.h"
#include "math/types.h"
#include "protocol.h"
#include "constants.h"
Crazyflie::Crazyflie(RadioDongle & radioDongle) :
    _radioDongle(radioDongle),
    _ackMissTolerance(100),
    _ackMissCounter(0),
    _sendSetPoint(),
    _maxSetPoint({45.0,45.0,180.0,60000}),
    _minThrust(0),
    _isSendingSetpoints(false),
    _isSendingVelocityRef(false),
    _startConnecting(false),
    _state (State::ZERO),
    _parameters(_radioDongle),
    _logger(_radioDongle),
    _leaveConnectingState(),
    _sensorValues()
{}

Crazyflie::~Crazyflie()
{
    _logger.Reset();
}
// Runs on 10ms.
void Crazyflie::Update()
{
    switch(_state)
    {
    case State::ZERO:
    {
        if(_startConnecting)
        {
            if( !_radioDongle.IsUsbConnectionOk()) // Can't connect if dongle is not ready
            {
                _state = State::ZERO;
                _startConnecting = false;
                emit NotConnecting();

            }
            else
            {
                _ackMissCounter = 0;
                _logger.Reset();
                _parameters.Reset();
                _state = State::SETUP_PARAMETERS;
            }
            _disconnect = false;
        }
        break;
    }
    case State::SETUP_PARAMETERS:
    {
        bool success = _parameters.Setup();
        if(success)
        {
            _state = State::READ_PARAMETERS;
        }
        break;
    }
    case State::READ_PARAMETERS:
    {
        bool success = _parameters.ReadAll();
        if(success)
        {
            _state = State::SETUP_LOGGER;
        }
        break;
    }
    case State::SETUP_LOGGER:
    {
        bool success = _logger.Setup();
        if(success)
        {
            _state = State::CREATE_LOGGERS;
        }
        break;
    }
    case State::CREATE_LOGGERS:
    {
        bool success = _logger.CreateLoggingBlocks();
        if(success)
        {
            _state = State::APPEND_LOGGERS;
        }
        break;
    }
    case State::APPEND_LOGGERS:
    {
        // Waits until the appending sequence is done.
        _logger.AppendLoggingBlocks();
        if(_logger.AppendingBlocksIsDone())
        {
            _state = State::START_LOGGERS;
        }
        break;
    }
    case State::START_LOGGERS:
    {
        bool success = _logger.EnableLogging();
        if(success)
        {
            _state = State::SET_PARAMETERS;
        }
        break;
    }
    case State::SET_PARAMETERS:
    {
        // Set Parameters that take into account the increased weight due to the camera
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::posCtlPid::thrustBase), 40000);
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::posCtlPid::thrustMin), 23000);
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vxKp), 25); // default 25
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vyKp), 25);// default 25
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vzKp), 25);// default 25
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vxKi), 1);// default 1
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vyKi), 1);// default 1
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vzKi), 1);// default 1
//        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vxKd), 0.1);// default 0
//        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vyKd), 0.1);// default 0
//        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::velCtlPid::vzKd), 0.1);// default 0


        _state = State::NORMAL_OPERATION;
    }
    case State::NORMAL_OPERATION:
    {
        if(_isSendingSetpoints)
        {
            // TODO SF: This must  be sent first to unlock the thrust
            //        SetPoint sp = {0.0, 0.0, 0.0, 0};
            //        SendSetpoint(sp);
            SendSetpoint(_sendSetPoint);
            _isSendingSetpoints = false;
        }
        else if(_isSendingSendPositionSetPoint)
        {
            // Send the current set point based on the previous calculations
            SendPositionSetPoint(_position_ref,_position_act);
        }
        else if(_isSendingVelocityRef)
        {
            SendVelocityRef(_velocity);
            _isSendingVelocityRef = false;
            // TODO SF: for debugging
            _logger.LogAll();
//            for(uint8_t i = 60; i < 80; ++i)
//            {
//                _logger.Log(i);
//            }
        }
//        else
//        {
//            SendSetpoint({0.0, 0.0, 0.0, 0});
//            SendVelocityRef({0.0,0.0,0.0});
//            SendPositionSetPoint({0.0,0.0,0.0},{0.0,0.0,0.0});
//        }

        if(_radioDongle.AckReceived())
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
            _state = State::DISCONNECT;
        }
        if(_disconnect)
        {
            _startConnecting = false;
            _state = State::DISCONNECT;
        }

        break;
    }
    case State::DISCONNECT:
    {
        _logger.Reset();
        _parameters.Reset();
        _state = State::ZERO;
        break;
    }
    default:
        break;
    } // end switch

    UpateSensorValues();
}

void Crazyflie::SendSetpoint(SetPoint setPoint)
{
    auto roll = setPoint.roll;
    auto pitch = setPoint.pitch;

    auto data = ConvertTouint8_tVect(roll);
    auto pitchVect = ConvertTouint8_tVect( -pitch); // Warning: Is negated here.
    auto yawVect = ConvertTouint8_tVect(setPoint.yaw);
    auto thrustVect = ConvertTouint8_tVect(setPoint.thrust);

    data.insert(data.end(), pitchVect.begin(), pitchVect.end());
    data.insert(data.end(), yawVect.begin(), yawVect.end());
    data.insert(data.end(), thrustVect.begin(), thrustVect.end());

    CRTPPacket  packet(Commander::id, Commander::Setpoint::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}

void Crazyflie::Stop()
{
    Data data;
    uint8_t command = CommanderGeneric::Channel::Stop::id;
    data.push_back(command);

    CRTPPacket  packet(CommanderGeneric::id, CommanderGeneric::Channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}

void  Crazyflie::SendVelocityRef(Velocity velocity)
{
    textLogger << "Sending velocity ref, x = " << velocity[0] << " y = " << velocity[1] << " z = " << velocity[2] << "\n";
    // vx in meter/s in world frame.
    // vy in meter/s in world frame.
    // vz in meter/s in world frame.
    // yawrate in deg/s

    Data data;
    auto vx_vect = ConvertTouint8_tVect(velocity[0]);
    auto vy_vect = ConvertTouint8_tVect(velocity[1]);
    auto vz_vect = ConvertTouint8_tVect(velocity[2]);
    auto yaw_vect = ConvertTouint8_tVect(0.0f);
    uint8_t command = CommanderGeneric::Channel::VelocityWorld::id;
    data.push_back(command);
    data.insert(data.end(), vx_vect.begin(), vx_vect.end());
    data.insert(data.end(), vy_vect.begin(), vy_vect.end());
    data.insert(data.end(), vz_vect.begin(), vz_vect.end());
    data.insert(data.end(), yaw_vect.begin(), yaw_vect.end());

    CRTPPacket  packet(CommanderGeneric::id, CommanderGeneric::Channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}
void  Crazyflie::SendPositionSetPoint(Point3f position_ref, Point3f position_act)
{
    textLogger << "Sending position ref, x = " << position_ref.x << " y = " << position_ref.y << " z = " << position_ref.z << "\n";
    textLogger << "Sending position act, x = " << position_act.x << " y = " << position_act.y << " z = " << position_act.z << "\n";
    // x in meter in world frame.
    // y in meter in world frame.
    // z in meter in world frame.
    // Actual sending
    Data data2;
    auto x_vect_act = ConvertTouint8_tVect(position_act.x);
    auto y_vect_act = ConvertTouint8_tVect(position_act.y);
    auto z_vect_act = ConvertTouint8_tVect(position_act.z);
//    uint8_t command2 = 0;
//    data2.push_back(command2);
    data2.insert(data2.end(), x_vect_act.begin(), x_vect_act.end());
    data2.insert(data2.end(), y_vect_act.begin(), y_vect_act.end());
    data2.insert(data2.end(), z_vect_act.begin(), z_vect_act.end());

    CRTPPacket  packet2(Localization::id, Localization::External_Position::id, std::move(data2));
    _radioDongle.RegisterPacketToSend(std::move(packet2));

    // Reference sending
    Data data;
    auto x_vect_ref = ConvertTouint8_tVect(position_ref.x);
    auto y_vect_ref = ConvertTouint8_tVect(position_ref.y);
    auto z_vect_ref = ConvertTouint8_tVect(position_ref.z);
    auto yaw_vect = ConvertTouint8_tVect(0.0f);
    uint8_t command = CommanderGeneric::Channel::Position::id;
    data.push_back(command);
    data.insert(data.end(), x_vect_ref.begin(), x_vect_ref.end());
    data.insert(data.end(), y_vect_ref.begin(), y_vect_ref.end());
    data.insert(data.end(), z_vect_ref.begin(), z_vect_ref.end());
    data.insert(data.end(), yaw_vect.begin(), yaw_vect.end());

    CRTPPacket  packet(CommanderGeneric::id, CommanderGeneric::Channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));

}



void  Crazyflie::SendHover(float vx, float vy, float yawrate, float zDistance)
{
    // vx in meter/s in body frame.
    // vy in meter/s in body frame.
    // yawrate in deg/s
    // zDistance meter/s in world frame.
    Data data;
    auto vx_vect = ConvertTouint8_tVect(vx);
    auto vy_vect = ConvertTouint8_tVect(vy);
    auto yaw_vect = ConvertTouint8_tVect(yawrate);
    auto zdist_vect = ConvertTouint8_tVect(zDistance);

    uint8_t command = CommanderGeneric::Channel::Hover::id;
    data.push_back(command);
    data.insert(data.end(), vx_vect.begin(), vx_vect.end());
    data.insert(data.end(), vy_vect.begin(), vy_vect.end());
    data.insert(data.end(), yaw_vect.begin(), yaw_vect.end());
    data.insert(data.end(), zdist_vect.begin(), zdist_vect.end());

    CRTPPacket packet(CommanderGeneric::id, CommanderGeneric::Channel::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}


void Crazyflie::StartConnecting(bool enable)
{
    _startConnecting = enable;
}
void Crazyflie::Disconnect(bool enable)
{
    _disconnect = enable;
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

void Crazyflie::SetSetPoint(SetPoint setPoint)
{
    SetThrust(setPoint.thrust);
    SetYaw(setPoint.yaw);
    SetRoll(setPoint.roll);
    SetPitch(setPoint.pitch);
}


void Crazyflie::SetVelocityWorldRef(Velocity velocity)
{
    // Set velocity in world coordinates. World coordinates are initialized at startup of the
    // crazyflie. At startup, the x-axis is in front diretion, and yaw = 0.
    // Velocity must be in meter/second
    _velocity = velocity;
}
void Crazyflie::SetVelocityCrazyFlieRef(Velocity velocity)
{
    // Set velocity in coordinate system relative to the crazyflie.
    // That is, x-diretion is always in front of the crazyflie.
    // Convert crazyflie coordinates into world coordinates by rotating the xy-plane.
    // The assumption is that the crazyflie is parallel to the ground.
    float cos_yaw = std::cos(_sensorValues.stabilizer.yaw * pi / 180.0f);
    float sin_yaw = std::sin(_sensorValues.stabilizer.yaw * pi / 180.0f);
    Velocity velocity_world;
    velocity_world[0] = cos_yaw * velocity[0] - sin_yaw * velocity[1];
    velocity_world[1] = sin_yaw * velocity[0] + cos_yaw * velocity[1];
    velocity_world[2] = velocity[2]; // z-axis is not changed
    SetVelocityWorldRef(velocity_world);
}

// TODO SF: Simplify setpoint setting
void Crazyflie::SetThrust(int thrust)
{
    _sendSetPoint.thrust = thrust;

    if(_sendSetPoint.thrust < _minThrust)
    {
        _sendSetPoint.thrust = _minThrust;
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

void Crazyflie::SetSendSetpoints(bool sendSetpoints)
{
    _isSendingSetpoints = sendSetpoints;
}
void Crazyflie::SetSendingVelocityRef(bool isSendingVelocityRef)
{
    _isSendingVelocityRef = isSendingVelocityRef;
}

void Crazyflie::SetPositionSetPoint(Point3f position_ref, Point3f position_act)
{
    _position_ref = position_ref;
    _position_act = position_act;
}
void Crazyflie::SetSendPositionSetPoint(bool isSendingSendPositionSetPoint)
{
    _isSendingSendPositionSetPoint = isSendingSendPositionSetPoint;
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


Eigen::Vector3f Crazyflie::ConvertBodyFrameToIntertialFrame(Eigen::Vector3f const & value_in_body)
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
    Eigen::Vector3f value_in_inertial;
    value_in_inertial[0] = cos_yaw * cos_pitch*x_b + (cos_yaw * sin_pitch * sin_roll -  sin_yaw  * cos_roll)*y_b + (cos_yaw*sin_pitch*cos_roll + sin_yaw*sin_roll)* z_b;
    value_in_inertial[1] = sin_yaw  * cos_pitch*x_b + (sin_yaw  * sin_pitch * sin_roll + cos_yaw * cos_roll)*y_b + (sin_yaw*sin_pitch*cos_roll  - cos_yaw*sin_roll)* z_b;
    value_in_inertial[2]  = -sin_pitch  * x_b + cos_pitch * sin_roll * y_b + cos_pitch * cos_roll*z_b;
    return value_in_inertial;
}

void Crazyflie::ResetCrazyflieKalmanFilter(bool enable)
{
    if(enable)
    {
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::kalman::resetEstimation), 0);
    }
    else
    {
        _parameters.WriteParameter(static_cast<uint8_t>(TocParameter::kalman::resetEstimation), 1);
    }
}
