#include "crazyfliecaller.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent) : _crazyFlie(crazyFlie), QObject(parent)
{
    // Execute Update() every 1ms. Add it to the event loop
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(Update()));
    _timer.start(1); // time in ms // TODO What does the intervall need to be??
}



void CrazyFlieCaller::Update()
{
    _crazyFlie.Update();
}
