#include "kalman_filter.h"

//KalmanFilter::KalmanFilter()
//{

//}



void KalmanFilter::update(Eigen::Vector3f input)
{
    // Currently only done for the first two dimensions
    Eigen::Vector2f measurement;
    measurement[0] = input [0];
    measurement[1] = input [1];

    // Prediction
    auto state_prediction = _A * _state_estimation;

    Eigen::Matrix4f f = _P * Eigen::Transpose<Eigen::Matrix4f>(_A);
    Eigen::Matrix4f a_posteriori_P = _A * f + _Q;
}
