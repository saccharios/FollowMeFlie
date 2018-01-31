#pragma once
#include "E:\Code\lib\eigen-3.3.4\Eigen/Core"
#include "E:\Code\lib\eigen-3.3.4\Eigen/LU"

#include "math/types.h"
using namespace Eigen;

template<typename T, unsigned int N_States, unsigned int N_Measurement>
class KalmanFilter
{

    using StateMatrix = Matrix<T, N_States, N_States>;
    using MeasMatrix = Matrix<T, N_Measurement, N_Measurement>;
    using State2MeasMatrix = Matrix<T, N_Measurement, N_States>;
    using Meas2StateMatrix = Matrix<T, N_States, N_Measurement>;
    using StateVector = Matrix<T,N_States,1>;
    using MeasVector = Matrix<T,N_Measurement,1>;

public:
    KalmanFilter () :
        _A(),
        _Q(),
        _H(),
        _H_transpose(),
        _R(),
        _P(),
        _state_estimation(StateVector::Zero(N_States,1))
    {
    }

    void SetStateUpdateMatrix(StateMatrix const & A)
    {
        _A = A;
    }
    void SetProcessNoiseCovMatrix(StateMatrix const & Q)
    {
        _Q = Q;
    }
    void SetMeasurementMatrix(State2MeasMatrix const & H)
    {
        _H = H;
        _H_transpose = _H.transpose();
    }
    void SetMeasurementNoiseCovMatrix(MeasMatrix const & R)
    {
        _R = R;
    }
    void SetCovMatrix(StateMatrix const & P)
    {
        _P = P;
    }

    StateVector update(MeasVector measurement)
    {
        // Prediction
        auto state_prediction = _A * _state_estimation;

        // Kalman gain
        StateMatrix a_posteriori_P =  _A*_P * _A.transpose() + _Q;
        MeasMatrix S = _H*a_posteriori_P*_H_transpose + _R;
        Meas2StateMatrix Gain = a_posteriori_P*_H_transpose * (S.inverse()); // Only use for matrices up to 4x4 !

        // Update
        _state_estimation = state_prediction + Gain *(measurement - _H*state_prediction);
        _P = (StateMatrix::Identity(N_States,N_States) - Gain * _H )*a_posteriori_P;
        return _state_estimation;
    }



private:
    StateMatrix _A;
    StateMatrix _Q;
    State2MeasMatrix _H;
    Meas2StateMatrix _H_transpose;
    MeasMatrix _R;
    StateMatrix _P;


    StateVector _state_estimation;

};



