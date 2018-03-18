#include "ball_kalman_filter.h"
#include "opencv_utils.h"
#include "camera.h"

cv::Point2f BallKalmanFilter::Update(std::vector<cv::KeyPoint> const & keyPoints)
{
    // Get best fit measurement
    // If the ball is larger than a threshold, this is taken
    auto largestKeyPoint = opencv_utils::GetLargestKeyPoint(keyPoints);

    static cv::Point2f pointFilteredOld = {0,0};
    if(largestKeyPoint.size == 0)
    {
        // If we don't have a measurement, we return the old filtered point
        // TODO SF Better: Let the kalman filter run, but with different matrices
        return pointFilteredOld;
    }

    // Update the kalman filter
    cv::Point2f pointFiltered = UpdateFilter(largestKeyPoint.pt);
    pointFilteredOld = pointFiltered;
    return pointFiltered;
}

cv::Point2f BallKalmanFilter::UpdateFilter(cv::Point2f pt)
{
    Eigen::Vector2f input = Eigen::Vector2f{pt.x, pt.y};
    Eigen::Vector4f state_estimate  = _kalmanFilter.Update(input);
    return cv::Point2f{state_estimate[0], state_estimate[1]};
}
