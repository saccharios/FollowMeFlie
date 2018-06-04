#include "ball_kalman_filter.h"
#include "opencv_utils.h"
#include "camera.h"
#include <opencv2\opencv.hpp>

Blob BallKalmanFilter::Update(std::vector<Blob> const & blobs)
{
    textLogger << "Update Kalman Filter\n";
    //    std::cout << "Num of blobs = " << blobs.size() << std::endl;
    // Predict
    Eigen::Vector4f prediction = _kalmanFilter.Predict();
    textLogger << "Preditction y = " << prediction[0]
               << " z = " << prediction[1]
               << " vx = " << prediction[2]
               << " vz = " << prediction[3] << "\n";

    // Get best fit measurement
    // If the ball is larger than a threshold, this is taken in any case
    Blob validMeasurement = GetLargestBlob(blobs);
    bool isValid = true;
    if(_validCounter < 20)
    {
        // If _validCounter >= 20, the best fit is the largest blob, and the KF is reset
        //        std::cout << "validMeasurement.size = " << validMeasurement.size << std::endl;
        isValid = GetBestFit(blobs, cv::Point2f{prediction[0],prediction[1]}, validMeasurement);
    }

    textLogger << "Best measurement,  y = " << validMeasurement.point.y << " z = " << validMeasurement.point.z << "\n";
    // Update the kalman filter
    Blob output = validMeasurement;
    Eigen::Vector4f estimate;
    if(isValid)
    {
        _validCounter = 0;
        estimate = UpdateFilter({validMeasurement.point.y,validMeasurement.point.z});
        //        std::cout << "Updating with valid measurement\n";
//        std::cout << _measurementNum << " Measurement, x =  " << output.point.y
//                  << " z = " <<  output.point.z
//                  << " size = " <<  output.size
//                  << std::endl;
        if( _measurementInProgress )
        {
            measurementAddition.point.y += output.point.y;
            measurementAddition.point.z += output.point.z;
            measurementAddition.size += output.size;

            ++_measurementNum;
            if(_measurementNum%(_maxMeasurementNum/10) == 0)
            {
                std::cout << _measurementNum << " Measurement, y =  " << output.point.y
                          << " z = " <<  output.point.z
                          << " size = " <<  output.size
                          << std::endl;
            }
            if(_measurementNum > _maxMeasurementNum)
            {
                _measurementNum = 0;
                _measurementInProgress = false;
                std::cout << "Measurement Average, y = " << measurementAddition.point.y/static_cast<float>(_maxMeasurementNum)
                          << " z = " <<  measurementAddition.point.z/static_cast<float>(_maxMeasurementNum)
                          << " size = " <<  measurementAddition.size/static_cast<float>(_maxMeasurementNum)
                          << std::endl;
                measurementAddition.point.y = 0;
                measurementAddition.point.z = 0;
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

    output.point.y = estimate[0];
    output.point.z = estimate[1];
    return output;
}

bool BallKalmanFilter::GetBestFit(std::vector<Blob> const & blobs, cv::Point2f prediction, Blob & bestFit)
{

    if(blobs.size() == 0)
    {
        return false;
    }

    // TODO SF:: Discount small measurementsa close to the edges, especially the corners

    bestFit = Blob();
    // Extract the one that is closest to the prediction
    double cost;
    int i = 0;
    for(auto const & blob : blobs)
    {
        cv::Point2f point{blob.point.y,blob.point.z};
        double twoNorm = cv::norm(cv::Mat(point - prediction), cv::NORM_L2SQR );

        // Assign cost at the start
        if(i == 0)
        {
            cost = twoNorm;
            bestFit = blob;
        }

        if(twoNorm < cost)
        {
            cost = twoNorm;
            bestFit = blob;
        }
        textLogger << "Blob " << i
                   << " x: " << blob.point.x
                   << " y: " << blob.point.y
                   << " z: " << blob.point.z << " cost = " << twoNorm<<"\n";
        ++i;
    }

    // Check if the closest is valid. We know the ball cannot travel too far in one sampling instant.
    if( cost > 10000 )
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

Eigen::Vector4f BallKalmanFilter::UpdateFilter(cv::Point2f pt)
{
    Eigen::Vector2f input = Eigen::Vector2f{pt.x, pt.y};
    _state_estimate  = _kalmanFilter.Estimate(input);
    return _state_estimate;
}

Eigen::Vector4f BallKalmanFilter::UpdateFilterNoMeas()
{
    _state_estimate  = _kalmanFilter.Estimate();
    return _state_estimate;
}

