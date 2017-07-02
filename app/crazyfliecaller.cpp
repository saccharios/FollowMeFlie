#include "crazyfliecaller.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent) :
    QObject(parent),
    _timer_t0(),
    _timer_t1(),
    _timer_t2(),
    _crazyFlie(crazyFlie)
{
    // Execute Update() every 10ms. Add it to the event loop:
    QObject::connect(&_timer_t0, SIGNAL(timeout()), this, SLOT(Update()));
    QObject::connect(&_timer_t2, SIGNAL(timeout()), this, SLOT(CheckConnectionTimeout()));
    _timer_t0.start(10); // time in ms
//    _timer_t1.start(100); // time in ms
    _timer_t2.start(500); // time in ms
}



void CrazyFlieCaller::Update()
{
    _crazyFlie.Update();
}

void CrazyFlieCaller::CheckConnectionTimeout()
{
    // TODO SF: Does not work reliable.
    if(_crazyFlie.IsConnectionTimeout())
    {
        std::cout << "conn timeout\n";
        emit ConnectionTimeout();
    }
}



