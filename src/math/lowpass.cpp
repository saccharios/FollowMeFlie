#include "lowpass.h"


double Lowpass::Update(double input)
{
    _state += _coefficient * (input - _state);
    return _state;
}
