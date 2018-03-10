#include "ball_kalman_filter.h"


Vector4f BallKalmanFilter::Update(Vector2f input)
{


    return _kalman_filter.Update(input);
}

