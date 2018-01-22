#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"
#include "math/types.h"

class KalmanFilter
{
public:
    KalmanFilter(float sampling_time) :
        _A(),
        _Q(),
        _H(),
        _R(),
        _P()
    {}

    void update(Eigen::Vector3f input);



private:
    Eigen::Matrix4f _A;
    Eigen::Matrix4f _Q;
    Eigen::Matrix<float, 2, 4> _H;
    Eigen::Matrix2f _R;
    Eigen::Matrix4f _P;


    Eigen::Vector3f _state_estimation;

};
