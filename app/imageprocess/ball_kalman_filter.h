#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"

#include "math/types.h"
#include "math/kalman_filter.h"


using namespace Eigen;

class BallKalmanFilter
{

public:
    BallKalmanFilter(float sampling_time, float meas_noise, float process_noise_1, float process_noise_2):
        _A(),
        _Q(),
        _H(),
        _R(),
        _kalman_filter(_A,_Q,_H,_R)
    {
//        // A =
//        // 1 0 Ts 0
//        // 0 1 0   Ts
//        // 0 0 1   0
//        // 0 0 0   1
//        _A = Matrix4f::Identity(4,4),
//        _A(0,2) = sampling_time;
//        _A(1,3) = sampling_time;
//        // Q =
//        // mn 0     pn2 0
//        // 0     mn 0     pn2
//        // pn2 0    pn1 0
//        // 0     pn2 0    pn1
//        _Q = Matrix4f::Zero(4,4);
//        _Q(0,0) = meas_noise;
//        _Q(1,1) = meas_noise;
//        _Q(2,2) = process_noise_1;
//        _Q(3,3) = process_noise_1;
//        _Q(2,0) = process_noise_2;
//        _Q(3,1) = process_noise_2;
//        _Q(0,2) = process_noise_2;
//        _Q(1,3) = process_noise_2;
//        // H =
//        // 1 0 0 0
//        // 0 1 0 0
//        _H = Matrix<float,2,4>::Zero(2,4);
//        _H(0,0) = 1;
//        _H(1,1) = 1;

//        // R =
//        // mn 0
//        // 0     mn
//        _R = Matrix4f::Identity(2,2)*meas_noise;

//        _kalman_filter = KalmanFilter<float,4,2>(_A, _Q, _H, _R); // must be last because the matrices are copied.

    }

    Vector4f update(Distance input){ return update(Vector2f{input.x, input.y});}
    Vector4f update(Vector3f input){ return update(Vector2f{input[0],input[1]});}
    Vector4f update(Vector2f input){ return _kalman_filter.update(input);}



private:
    Matrix4f _A;
    Matrix4f _Q;
    Matrix<float, 2, 4> _H;
    Matrix2f _R;
//    Matrix4f _P;
    KalmanFilter<float,4,2> _kalman_filter;
};
