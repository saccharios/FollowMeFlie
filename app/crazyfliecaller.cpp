#include "crazyfliecaller.h"

CrazyFlieCaller::CrazyFlieCaller(Crazyflie & crazyFlie, QObject *parent) : _crazyFlie(crazyFlie), QObject(parent)
{
    // Execute Update() every 1ms. Add it to the event loop
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(Update()));
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(CheckRollChanged()));
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(CounterUpdate()));
    _timer.start(200); // time in ms // TODO What does the intervall need to be??
}



void CrazyFlieCaller::Update()
{
    _crazyFlie.Update();
}

void CrazyFlieCaller::CounterUpdate()
{
    ++_counter;
    emit CounterSignal();
}
int CrazyFlieCaller::Counter() const
{
    return _counter;
}

void CrazyFlieCaller::CheckRollChanged()
{
    // TODO Not getting the actual roll with _crazyFlie.GetRoll()
    std::cout << "_crazyFlie.GetRoll() = " << _crazyFlie.GetRoll() << std::endl;
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

