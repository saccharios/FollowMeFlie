#pragma once
#include <QGenericMatrix>
#include "math/types.h"
class ExtendedKalmanFilter
{
public:
    ExtendedKalmanFilter();

    // TODO SF As a start, the H_k matrix is constant. It actually can vary, depending on when which new measurementa are available.
    // A good idea is to update the ekf when at least gyro measurement is available,.
    void update();


private:
   Matrix12 Process_Covariance_Q;

};
