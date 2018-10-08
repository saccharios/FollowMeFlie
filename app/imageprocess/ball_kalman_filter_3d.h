#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"

#include "math/types.h"
#include "math/kalman_filter.h"
#include "opencv2/opencv.hpp"

using namespace Eigen;

class BallKalmanFilter_3d
{
public:
    static constexpr unsigned int num_states = 6;
    static constexpr unsigned int num_measurements = 3;
    using Vector6f = Matrix<float,num_states,1>;
    using Matrix6f = Matrix<float,num_states,num_states>;
    // State is defined as (x, y, z, vx, vy, vz)
    BallKalmanFilter_3d(float meas_noise, float process_noise_1, float process_noise_2):
        _A(),
        _Q(),
        _H(),
        _R(),
        _kalmanFilter(),
        _state_estimate()
    {
        // A =
        // 1 0 0 1 0 0
        // 0 1 0 0 1 0
        // 0 0 1 0 0 1
        // 0 0 0 1 0 0
        // 0 0 0 0 1 0
        // 0 0 0 0 0 1
        _A = Matrix<float,num_states,num_states>::Identity(num_states,num_states);
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
        _Q = Matrix<float,num_states,num_states>::Identity(num_states,num_states)*process_noise_1;
        _Q(3,0) = process_noise_2;
        _Q(4,1) = process_noise_2;
        _Q(5,2) = process_noise_2;
        _Q(0,3) = process_noise_2;
        _Q(1,4) = process_noise_2;
        _Q(2,5) = process_noise_2;
        // H =
        // 1 0 0 0 0 0
        // 0 1 0 0 0 0
        // 0 0 1 0 0 0
        _H = Matrix<float,num_measurements,num_states>::Zero(num_measurements,num_states);
        _H(0,0) = 1;
        _H(1,1) = 1;
        _H(2,2) = 1;

        // R =
        // mn 0  0
        // 0  mn 0
        // 0  0  mn
        _R = Matrix3f::Identity(num_measurements,num_measurements)*meas_noise;


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
    BallKalmanFilter_3d::Vector6f Update(std::vector<Point3f> const & crazyFliePoints);
private:
    Matrix6f _A;
    Matrix6f _Q;
    Matrix<float, num_measurements, num_states> _H;
    Matrix3f _R;
    //    Matrix6f _P; // Initial estimat of covariane matrix P not used
    KalmanFilter<float,num_states,num_measurements> _kalmanFilter;

    Vector6f _state_estimate;
    Vector6f UpdateFilter(Point3f pt);
    Vector6f UpdateFilterNoMeas();
    unsigned int _validCounter = 0;
    bool GetBestFit(std::vector<Point3f> const & crazyFliePoints, cv::Point3f prediction, Point3f & bestFit);
};
