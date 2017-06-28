#pragma once
#include <assert.h>
#include <cmath>

template<typename T>
T WrapAround(T value, T min, T max)
{
    assert(max > min);
    auto diff = max -min;
    auto a = fmod(value,diff);
    if( a > max)
    {
        a -= diff;
    }
    else if( a <= min)
    {
        a += diff;
    }
    return a;
}
