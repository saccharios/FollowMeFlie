#include "crazyfliecaller.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent) : _crazyFlie(crazyFlie), QObject(parent)
{
    // Execute Update() every 10ms. Add it to the event loop:
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(Update()));
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(CheckRollChanged()));
    _timer.start(10); // time in ms // TODO What does the intervall need to be??
}



void CrazyFlieCaller::Update()
{
    _crazyFlie.Update();
}

void CrazyFlieCaller::CheckRollChanged()
{
    if(_crazyFlie.GetRoll() != _roll)
    {
        _roll = _crazyFlie.GetRoll();
        emit RollChanged();
    }
}

float CrazyFlieCaller::GetRoll() const
{
    return _roll;
}

