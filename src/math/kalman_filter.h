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
        _A(StateMatrix::Zero()),
        _Q(StateMatrix::Zero()),
        _H(State2MeasMatrix::Zero()),
        _H_transpose(Meas2StateMatrix::Zero()),
        _R(MeasMatrix::Zero()),
        _P(StateMatrix::Zero()),
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

    void Initialize(MeasVector measurement, StateMatrix const & P = StateMatrix::Identity(N_States,N_States) )
    {
        _state_estimation[0] = measurement[0];
        _state_estimation[1] = measurement[1];
        _state_estimation[2] = 0;
        _state_estimation[3] = 0;
        _P = P;
    }

    StateVector Update(MeasVector measurement)
    {

        std::cout << "measurement = " << measurement[0] << " "
                  << measurement[1] << "\n";
        // Prediction
        auto state_prediction = _A * _state_estimation;

//        std::cout << "prediction = " << state_prediction[0] << " "
//                     << state_prediction[1] << " "
//                        << state_prediction[2] << " "
//                           << state_prediction[3] << "\n";


//        std::cout << "_P = "  << _P << std::endl;
//        std::cout << "_A = "  << _A << std::endl;
//        std::cout << "_A.transpose()  = "  << _A.transpose()  << std::endl;
//        std::cout << "_Q = "  << _Q << std::endl;

        // Kalman gain
        StateMatrix a_posteriori_P =  _A*_P * _A.transpose() + _Q;
        //std::cout << "a_posteriori_P = "  << a_posteriori_P << std::endl;
        MeasMatrix S = _H*a_posteriori_P*_H_transpose + _R;
        Meas2StateMatrix Gain = a_posteriori_P*_H_transpose * (S.inverse()); // Only use for matrices up to 4x4 !

        // Update
        _state_estimation = state_prediction + Gain *(measurement - _H*state_prediction);
        _P = (StateMatrix::Identity(N_States,N_States) - Gain * _H )*a_posteriori_P;

//        std::cout << "_state_estimation = " << _state_estimation[0] << " "
//                     << _state_estimation[1] << " "
//                        << _state_estimation[2] << " "
//                           << _state_estimation[3] << "\n";
//        std::cout << "---------------------------------------------\n";

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



