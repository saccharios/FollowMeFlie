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
    QObject::connect(&_timer_t0, SIGNAL(timeout()), this, SLOT(Update()));
    _timer_t0.start(crazyflieUpdateSamplingTime); // time in ms
    _timer_t2.start(500); // time in ms
}



void CrazyFlieCaller::Update()
{
    _commander.Update();
    _crazyFlie.Update();
}




