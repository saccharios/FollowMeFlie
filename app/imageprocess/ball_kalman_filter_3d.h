#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"

#include "math/types.h"
#include "math/kalman_filter.h"
#include "opencv2/opencv.hpp"

using namespace Eigen;

class BallKalmanFilter_3d
{
    static constexpr unsigned int num_states = 6;
    static constexpr unsigned int num_measurements = 3;
    using Vector6f = Matrix<float,num_states,1>;
    // State is defined as (x, y, dx, dy)
public:
    BallKalmanFilter_3d(float meas_noise, float process_noise_1, float process_noise_2):
        _A(),
        _Q(),
        _H(),
        _R(),
        _kalmanFilter(),
        _state_estimate(),
        _previous_valid_size(0.0f)
    {
        // A =
        // 1 0 0 1 0 0
        // 0 1 0 0 1 0
        // 0 0 1 0 0 1
        // 0 0 0 1 0 0
        // 0 0 0 0 1 0
        // 0 0 0 0 0 1
        _A = Matrix4f::Identity(num_states,num_states);
        _A(0,3) = 1;
        _A(1,4) = 1;
        _A(2,5) = 1;
        // Q =
        // pn1 0   0   pn2 0   0
        // 0   pn1 0   0   pn2 0
        // 0   0   pn1 0   0   pn2
        // pn2 0   0   pn1 0   0
        // 0   pn2 0   0   pn1 0
        // 0   0   pn2 0   0   pn1
        _Q = Matrix4f::Zero(num_states,num_states);
        _Q(0,0) = process_noise_1;
        _Q(1,1) = process_noise_1;
        _Q(2,2) = process_noise_1;
        _Q(3,3) = process_noise_1;
        _Q(2,0) = process_noise_2;
        _Q(3,1) = process_noise_2;
        _Q(0,2) = process_noise_2;
        _Q(1,3) = process_noise_2;
        // H =
        // 1 0 0 0 0
        // 0 1 0 0 0
        // 0 0 1 0 0
        _H = Matrix<float,num_measurements,num_states>::Zero(2,num_states);
        _H(0,0) = 1;
        _H(1,1) = 1;
        _H(2,2) = 1;

        // R =
        // mn 0  0
        // 0  mn 0
        // 0  0  mn
        _R = Matrix2f::Identity(num_measurements,num_measurements)*meas_noise;


        _kalmanFilter.SetStateUpdateMatrix(_A);
        _kalmanFilter.SetProcessNoiseCovMatrix(_Q);
        _kalmanFilter.SetMeasurementMatrix(_H);
        _kalmanFilter.SetMeasurementNoiseCovMatrix(_R);

//        textLogger << _A<< "\n-----\n";
//        textLogger << _Q<< "\n-----\n";
//        textLogger << _H<< "\n-----\n";
//        textLogger << _R << "\n-----\n";
    }


    void Initialize(Point3f input) {_kalmanFilter.Initialize(Vector3f{input.x, input.y, input.z});}
    Point3f Update(std::vector<Point3f> const & crazyFliePoints);
private:
    Matrix4f _A;
    Matrix4f _Q;
    Matrix<float, num_measurements, num_states> _H;
    Matrix2f _R;
    //    Matrix4f _P; // Initial estimat of covariane matrix P not used
    KalmanFilter<float,num_states,2> _kalmanFilter;

    Vector6f _state_estimate;
    Vector6f UpdateFilter(Point3f pt);
    Vector6f UpdateFilterNoMeas();
    unsigned int _validCounter = 0;
    bool GetBestFit(std::vector<Point3f> const & crazyFliePoints, cv::Point3f prediction, Point3f & bestFit);
};
