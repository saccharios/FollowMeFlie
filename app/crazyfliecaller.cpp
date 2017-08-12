#include "crazyfliecaller.h"
#include "communication/Crazyflie.h"
#include "control/commander.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, Commander & commander, QObject *parent) :
    QObject(parent),
    _timer_t0(),
    _timer_t2(),
    _crazyFlie(crazyFlie),
    _commander(commander)
{
    // Execute Update() every 10ms. Add it to the event loop:
    QObject::connect(&_timer_t0, SIGNAL(timeout()), this, SLOT(Update()));
    QObject::connect(&_timer_t2, SIGNAL(timeout()), this, SLOT(CheckConnectionTimeout()));
    _timer_t0.start(10); // time in ms
    _timer_t2.start(500); // time in ms
}



void CrazyFlieCaller::Update()
{
    _commander.Update();
    _crazyFlie.Update();
}

void CrazyFlieCaller::CheckConnectionTimeout()
{
    if(_crazyFlie.IsConnectionTimeout())
    {
        std::cout << "conn timeout\n";
        emit ConnectionTimeout();
    }
}



