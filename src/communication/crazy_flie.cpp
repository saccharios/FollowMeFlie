#include "crazy_flie.h"
#include <chrono>
#include "math/constants.h"
#include "math/types.h"
#include "protocol.h"

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
    // TODO SF Stop logging and reset everything on the crazyflie.
}
// Runs on 10ms.
void Crazyflie::Update()
{
    // TODO SF: Alternative way of implementing state machine

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
        if( !_radioDongle.IsUsbConnectionOk())
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
            _logger.ResetLoggingBlocks(); // Prepare to create logging blocks
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
            _state = State::ZERO_MEASUREMENTS;
        }
        break;
    }
    case State::ZERO_MEASUREMENTS:
    {
        // TODO SF : Remove this state
        // NOTE : ter;
        _state = State::NORMAL_OPERATION;
        break;
    }
    case State::NORMAL_OPERATION:
    {
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
            _state = State::ZERO;
        }

        break;
    }
    default:
        break;
    } // end switch

    UpateSensorValues();
}

void Crazyflie::SendSetpoint(SetPoint setPoint)
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

    CRTPPacket  packet(Commander::id, Commander::Setpoint::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}

void  Crazyflie::SendVelocityRef(Velocity velocity)
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

    CRTPPacket  packet(CommanderGeneric::id, CommanderGeneric::GenericSetpoint::id, std::move(data));
    _radioDongle.RegisterPacketToSend(std::move(packet));
}


void Crazyflie::StartConnecting(bool enable)
{
    _startConnecting = enable;
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

