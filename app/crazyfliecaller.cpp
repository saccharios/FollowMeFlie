#include "crazyfliecaller.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent) : _crazyFlie(crazyFlie), QObject(parent)
{
    // Execute Update() every 10ms. Add it to the event loop:
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(Update()));
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(CheckActValueChanged()));
    _timer.start(10); // time in ms // TODO What does the intervall need to be??
}



void CrazyFlieCaller::Update()
{
    _crazyFlie.Update();
}


void CrazyFlieCaller::CheckActValueChanged()
{
    if(_crazyFlie.GetThrust() != _thrust ||
            _crazyFlie.GetPitch() != _pitch ||
            _crazyFlie.GetYaw() != _yaw ||
            _crazyFlie.GetRoll() != _roll
            )
    {
        _thrust= _crazyFlie.GetThrust();
        _roll = _crazyFlie.GetRoll();
         _yaw = _crazyFlie.GetYaw();
        _pitch = _crazyFlie.GetPitch();
        emit ActValueChanged();
    }
}

float CrazyFlieCaller::GetRoll() const
{
    return _roll;
}
float CrazyFlieCaller::GetYaw() const
{
    return _yaw;
}
float CrazyFlieCaller::GetPitch() const
{
    return _pitch;
}
int CrazyFlieCaller::GetThrust() const
{
    return _thrust;
}

