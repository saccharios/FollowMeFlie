#include "ball_kalman_filter_2d.h"
#include "opencv_utils.h"
#include "camera.h"
#include <opencv2\opencv.hpp>
#include "math/types.h"
MidPoint BallKalmanFilter_2d::Update(std::vector<MidPoint> const & midPoints)
{
//    textLogger << "Update Kalman Filter\n";
    //    std::cout << "Num of midPoints = " << midPoints.size() << std::endl;
    // Predict
    Eigen::Vector4f prediction = _kalmanFilter.Predict();
//    textLogger << "Preditction y = " << prediction[0]
//               << " z = " << prediction[1]
//               << " vx = " << prediction[2]
//               << " vz = " << prediction[3] << "\n";

    // Get best fit measurement
    // If the ball is larger than a threshold, this is taken in any case
    MidPoint validMeasurement = GetLargest(midPoints);
    bool isValid = true;
    if(_validCounter < 20)
    {
        // If _validCounter >= 20, the best fit is the largest midPoints, and the KF is reset
        //        std::cout << "validMeasurement.size = " << validMeasurement.size << std::endl;
        isValid = GetBestFit(midPoints, cv::Point2f{prediction[0],prediction[1]}, validMeasurement);
    }

//    textLogger << "Best measurement,  x = " << validMeasurement.pt.x << " y = " << validMeasurement.pt.y << "\n";
    // Update the kalman filter
    MidPoint output = validMeasurement;
    Eigen::Vector4f estimate;
    if(isValid)
    {
        _validCounter = 0;
        estimate = UpdateFilter({validMeasurement.pt.x,validMeasurement.pt.y});
        _previous_valid_size = validMeasurement.size;
        //        std::cout << "Updating with valid measurement\n";
//        std::cout << _measurementNum << " Measurement, x =  " << output.point.y
//                  << " z = " <<  output.point.z
//                  << " size = " <<  output.size
//                  << std::endl;
        if( _measurementInProgress )
        {
            measurementAddition.pt.x += output.pt.x;
            measurementAddition.pt.y += output.pt.y;
            measurementAddition.size += output.size;

            ++_measurementNum;
            if(_measurementNum%(_maxMeasurementNum/10) == 0)
            {
                std::cout << _measurementNum << " Measurement, x =  " << output.pt.x
                          << " y = " <<  output.pt.y
                          << " size = " <<  output.size
                          << std::endl;
            }
            if(_measurementNum > _maxMeasurementNum)
            {
                _measurementNum = 0;
                _measurementInProgress = false;
                std::cout << "Measurement Average, x = " << measurementAddition.pt.x/static_cast<float>(_maxMeasurementNum)
                          << " y = " <<  measurementAddition.pt.y/static_cast<float>(_maxMeasurementNum)
                          << " size = " <<  measurementAddition.size/static_cast<float>(_maxMeasurementNum)
                          << std::endl;
                measurementAddition.pt.x = 0;
                measurementAddition.pt.y = 0;
                measurementAddition.size = 0;
            }
        }
    }
    else
    {
        ++_validCounter;
        estimate = UpdateFilterNoMeas();
        //        std::cout << "Updating with NON valid measurement\n";
    }
    //    std::cout << "estimate, y = " << estimate[0]
    //              << " z = " << estimate[1]
    //            << " vy = " << estimate[2]
    //            << " vz = " << estimate[3] << std::endl;

    output.pt.x = estimate[0];
    output.pt.y = estimate[1];
    output.size = _previous_valid_size;
    return output;
}

bool BallKalmanFilter_2d::GetBestFit(std::vector<MidPoint> const & midPoints, cv::Point2f prediction, MidPoint & bestFit)
{

    if(midPoints.size() == 0)
    {
        return false;
    }

    // TODO SF:: Discount small measurementsa close to the edges, especially the corners

    bestFit = MidPoint();
    // Extract the one that is closest to the prediction
    double max_cost = 10000;
    double cost = max_cost + 1;
    int i = 0;
    for(auto const & midPoint : midPoints)
    {
        cv::Point2f point{midPoint.pt.x,midPoint.pt.y};
        double twoNorm = cv::norm(cv::Mat(point - prediction), cv::NORM_L2SQR );

        // Assign cost at the start
        if(i == 0)
        {
            cost = twoNorm;
            bestFit = midPoint;
        }

        if(twoNorm < cost)
        {
            cost = twoNorm;
            bestFit = midPoint;
        }
//        textLogger << "MidPoint " << i
//                   << " x: " << midPoint.pt.x
//                   << " y: " << midPoint.pt.y
//                   << " size: " << midPoint.size
//                   << " cost = " << twoNorm<<"\n";
        ++i;
    }

    // Check if the closest is valid. We know the ball cannot travel too far in one sampling instant.
    if( cost > max_cost )
    {
//        std::cout << "Out of reach, cost = " << cost << "y_pred =  "
//                  << prediction.x << " z_pred = "
//                  << prediction.y  << " y_best = "
//                  << bestFit.point.y << " z_best="
//                  << bestFit.point.z<< std::endl;
        return false;
    }
    return true;
}

Eigen::Vector4f BallKalmanFilter_2d::UpdateFilter(cv::Point2f pt)
{
    Eigen::Vector2f input = Eigen::Vector2f{pt.x, pt.y};
    _state_estimate  = _kalmanFilter.Estimate(input);
    return _state_estimate;
}

Eigen::Vector4f BallKalmanFilter_2d::UpdateFilterNoMeas()
{
    _state_estimate  = _kalmanFilter.Estimate();
    return _state_estimate;
}

