#include "crazyfliecaller.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent) :
    QObject(parent),
    _timerUpdate(),
    _timerDisplay(),
    _crazyFlie(crazyFlie)
{
    // Execute Update() every 10ms. Add it to the event loop:
    QObject::connect(&_timerUpdate, SIGNAL(timeout()), this, SLOT(Update()));
    QObject::connect(&_timerDisplay, SIGNAL(timeout()), this, SLOT(UpdateActValueTime()));
    _timerUpdate.start(10); // time in ms // TODO What does the intervall need to be??
    _timerDisplay.start(100); // time in ms // TODO What does the intervall need to be??
}



void CrazyFlieCaller::Update()
{
    _crazyFlie.Update();
}

void CrazyFlieCaller::UpdateActValueTime()
{
    emit UpdateActValues();
}

float CrazyFlieCaller::GetRoll() const
{
    return _crazyFlie.GetRoll();
}
float CrazyFlieCaller::GetYaw() const
{
    return _crazyFlie.GetYaw();
}
float CrazyFlieCaller::GetPitch() const
{
    return _crazyFlie.GetPitch();
}
int CrazyFlieCaller::GetThrust() const
{
    return _crazyFlie.GetThrust();
}

