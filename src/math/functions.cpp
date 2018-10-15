#include "functions.h"
float deg2rad(float deg)
{
    return WrapAround(deg/180.0f*pi, -pi, pi);
}

Point3f ConvertToPosition( Point3f error)
{
    return {error.x, -error.y, -error.z};
}
