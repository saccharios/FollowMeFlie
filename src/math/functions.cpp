#include "functions.h"
float deg2rad(float deg)
{
    return WrapAround(deg/180.0f*pi, -pi, pi);
}

