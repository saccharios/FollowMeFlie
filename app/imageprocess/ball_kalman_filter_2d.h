#pragma once
#include </usr/include/eigen3/Eigen/Core>

#include "math/types.h"
#include "math/kalman_filter.h"
#include "opencv2/opencv.hpp"

using namespace Eigen;

class BallKalmanFilter_2d
{

    // State is defined as (x, y, dx, dy)
public:
    BallKalmanFilter_2d(float meas_noise, float process_noise_1, float process_noise_2):
        _A(),
        _Q(),
        _H(),
        _R(),
        _kalmanFilter(),
        _state_estimate(),
        _previous_valid_size(0.0f)
    {
        // A =
        // 1 0 1 0
        // 0 1 0 1
        // 0 0 1 0
        // 0 0 0 1
        _A = Matrix4f::Identity(4,4);
        _A(0,2) = 1;
        _A(1,3) = 1;
        // Q =
        // pn1 0     pn2 0
        // 0     pn1 0     pn2
        // pn2 0    pn1 0
        // 0     pn2 0    pn1
        _Q = Matrix4f::Zero(4,4);
        _Q(0,0) = process_noise_1;
        _Q(1,1) = process_noise_1;
        _Q(2,2) = process_noise_1;
        _Q(3,3) = process_noise_1;
        _Q(2,0) = process_noise_2;
        _Q(3,1) = process_noise_2;
        _Q(0,2) = process_noise_2;
        _Q(1,3) = process_noise_2;
        // H =
        // 1 0 0 0
        // 0 1 0 0
        _H = Matrix<float,2,4>::Zero(2,4);
        _H(0,0) = 1;
        _H(1,1) = 1;

        // R =
        // mn 0
        // 0  mn
        _R = Matrix2f::Identity(2,2)*meas_noise;


        _kalmanFilter.SetStateUpdateMatrix(_A);
        _kalmanFilter.SetProcessNoiseCovMatrix(_Q);
        _kalmanFilter.SetMeasurementMatrix(_H);
        _kalmanFilter.SetMeasurementNoiseCovMatrix(_R);

//        textLogger << _A<< "\n-----\n";
//        textLogger << _Q<< "\n-----\n";
//        textLogger << _H<< "\n-----\n";
//        textLogger << _R << "\n-----\n";
    }


    void Initialize(MidPoint input) {_kalmanFilter.Initialize(Vector2f{input.pt.x, input.pt.y});}
    MidPoint Update(std::vector<MidPoint> const & midPoints);
    void StartMeasurement(bool start) {_measurementInProgress = start;}
private:
    Matrix4f _A;
    Matrix4f _Q;
    Matrix<float, 2, 4> _H;
    Matrix2f _R;
    //    Matrix4f _P; // Initial estimat of covariane matrix P not used
    KalmanFilter<float,4,2> _kalmanFilter;

    Eigen::Vector4f _state_estimate;
    float _previous_valid_size;
    Eigen::Vector4f UpdateFilter(cv::Point2f pt);
    Eigen::Vector4f UpdateFilterNoMeas();
    unsigned int _validCounter = 0;
    bool GetBestFit(std::vector<MidPoint> const & midPoints, cv::Point2f prediction, MidPoint & bestFit);
    bool _measurementInProgress = false;
    int _measurementNum = 0;
    static constexpr int _maxMeasurementNum = 500;
    MidPoint measurementAddition;
};
