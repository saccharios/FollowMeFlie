#include "ball_kalman_filter.h"
#include "opencv_utils.h"
#include "camera.h"
#include <opencv2\opencv.hpp>

Blob BallKalmanFilter::Update(std::vector<Blob> const & blobs)
{
    textLogger << "Update Kalman Filter\n";
    // Predict
    Eigen::Vector4f prediction = _kalmanFilter.Predict();
    textLogger << "Preditction y = " << prediction[0]
               << " z = " << prediction[1]
               << " vx = " << prediction[2]
               << " vz = " << prediction[3] << "\n";

    // Get best fit measurement
    // If the ball is larger than a threshold, this is taken
    Blob validMeasurement = GetLargestBlob(blobs);
    bool isValid = true;
    // Special routine to get the best fit if the ball is far away
    // If there was no measurement 20 consecutive times, the largestKeyPoint is taken.
    if((validMeasurement.size < 20) && (_validCounter < 20))
    {
        std::cout << "getting the best fit\n";
        isValid = GetBestFit(blobs, cv::Point2f{prediction[0],prediction[1]}, validMeasurement);
    }
    else
    {
        std::cout << "yeah, this is not working!\n";
    }

    textLogger << "Best measurement,  y = " << validMeasurement.point.y << " z = " << validMeasurement.point.z << "\n";
    // Update the kalman filter
    Blob output = validMeasurement;
    cv::Point2f estimate;
    if(isValid)
    {
        _validCounter = 0;
        estimate = UpdateFilter({validMeasurement.point.y,validMeasurement.point.z});
        textLogger << "Updating with valid measurement\n";
    }
    else
    {
        ++_validCounter;
        estimate = UpdateFilterNoMeas();
        textLogger << "Updating with NON valid measurement\n";
    }
    output.point.y = estimate.x;
    output.point.z = estimate.y;
    return output;
}

bool BallKalmanFilter::GetBestFit(std::vector<Blob> const & blobs, cv::Point2f prediction, Blob & bestFit)
{
    // TODO SF:: Discount small measurementsa close to the edges, especially the corners


    // Extract the one that is closest to the prediction
    double cost = 99999999;
    int i = 0;
    for(auto const & blob : blobs)
    {
        cv::Point2f point{blob.point.y,blob.point.z};
        double twoNorm = cv::norm(cv::Mat(point - prediction), cv::NORM_L2SQR );
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
    if( cost > 400 )
    {
        return false;
    }
    return true;
}

cv::Point2f BallKalmanFilter::UpdateFilter(cv::Point2f pt)
{
    Eigen::Vector2f input = Eigen::Vector2f{pt.x, pt.y};
    Eigen::Vector4f state_estimate  = _kalmanFilter.Estimate(input);
    return cv::Point2f{state_estimate[0], state_estimate[1]};
}

cv::Point2f BallKalmanFilter::UpdateFilterNoMeas()
{
    Eigen::Vector4f state_estimate  = _kalmanFilter.Estimate();
    return cv::Point2f{state_estimate[0], state_estimate[1]};
}

