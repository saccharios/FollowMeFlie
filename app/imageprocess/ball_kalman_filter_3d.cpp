#include "ball_kalman_filter_3d.h"
#include "opencv_utils.h"
#include "camera.h"
#include <opencv2\opencv.hpp>
#include "math/types.h"
Point3f BallKalmanFilter_3d::Update(std::vector<Point3f> const & crazyFliePoints)
{
//    textLogger << "Update Kalman Filter\n";
    //    std::cout << "Num of midPoints = " << midPoints.size() << std::endl;
    // Predict
    BallKalmanFilter_3d::Vector6f prediction = _kalmanFilter.Predict();
//    textLogger << "Preditction y = " << prediction[0]
//               << " z = " << prediction[1]
//               << " vx = " << prediction[2]
//               << " vz = " << prediction[3] << "\n";

    // Get best fit measurement
    // If the ball is larger than a threshold, this is taken in any case
    Point3f validMeasurement = GetLargest(crazyFliePoints); // TODO SF
    bool isValid = true;
    if(_validCounter < 20)
    {
        // If _validCounter >= 20, the best fit is the largest midPoints, and the KF is reset
        //        std::cout << "validMeasurement.size = " << validMeasurement.size << std::endl;
        isValid = GetBestFit(crazyFliePoints, cv::Point3f{prediction[0],prediction[1],prediction[2]}, validMeasurement);
    }

//    textLogger << "Best measurement,  x = " << validMeasurement.pt.x << " y = " << validMeasurement.pt.y << "\n";
    // Update the kalman filter
    Point3f output = validMeasurement;
    BallKalmanFilter_3d::Vector6f estimate;
    if(isValid)
    {
        _validCounter = 0;
        estimate = UpdateFilter(validMeasurement);
        //        std::cout << "Updating with valid measurement\n";
//        std::cout << _measurementNum << " Measurement, x =  " << output.point.y
//                  << " z = " <<  output.point.z
//                  << " size = " <<  output.size
//                  << std::endl;
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

    output.x = estimate[0];
    output.y = estimate[1];
    output.z = estimate[2];
    return output;
}

bool BallKalmanFilter_3d::GetBestFit(std::vector<Point3f> const & crazyFliePoints, cv::Point3f prediction, Point3f & bestFit)
{

    if(crazyFliePoints.size() == 0)
    {
        return false;
    }

    // TODO SF:: Discount small measurementsa close to the edges, especially the corners

    bestFit = Point3f();
    // Extract the one that is closest to the prediction
    double max_cost = 10000;
    double cost = max_cost + 1;
    int i = 0;
    for(auto const & crazyFliePoint : crazyFliePoints)
    {
        cv::Point3f point{crazyFliePoint.x,crazyFliePoint.y,crazyFliePoint.z};
        double twoNorm = cv::norm(cv::Mat(point - prediction), cv::NORM_L2SQR );

        // Assign cost at the start
        if(i == 0)
        {
            cost = twoNorm;
            bestFit = crazyFliePoint;
        }

        if(twoNorm < cost)
        {
            cost = twoNorm;
            bestFit = crazyFliePoint;
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

BallKalmanFilter_3d::Vector6f BallKalmanFilter_3d::UpdateFilter(Point3f pt)
{
    Eigen::Vector3f input = Eigen::Vector3f{pt.x, pt.y, pt.z};
    _state_estimate  = _kalmanFilter.Estimate(input);
    return _state_estimate;
}

BallKalmanFilter_3d::Vector6f BallKalmanFilter_3d::UpdateFilterNoMeas()
{
    _state_estimate  = _kalmanFilter.Estimate();
    return _state_estimate;
}

