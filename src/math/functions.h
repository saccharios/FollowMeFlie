#pragma once
#include <assert.h>
#include <cmath>
#include "constants.h"
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


float deg2rad(float deg);


template <typename T>
T Limit(T input, T min, T max)
{
    if(input > max)
    {
        return max;
    }
    else if (input < min)
    {
        return min;
    }
    else
    {
        return input;
    }
}



