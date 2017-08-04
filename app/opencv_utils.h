#include "opencv2/opencv.hpp"

#pragma once

namespace cv_utils
{
cv::KeyPoint GetLargestKeyPoint(std::vector<cv::KeyPoint> const & keypoints)
{
    cv::KeyPoint largestKeyPoint;
    if(keypoints.size() == 1)
    {
        largestKeyPoint =  keypoints.at(0);
    }
    else if(keypoints.size() > 1)
    {
        for(auto const & keyPoint : keypoints)
        {
            if(keyPoint.size > largestKeyPoint.size)
            {
                largestKeyPoint = keyPoint;
            }
        }
    }
    return largestKeyPoint;
}


cv::Point2f  ConvertCameraToMidPointCoord(cv::Point2f cameraPt, cv::Size size)
{
    cv::Point2f midPointCoord;
    midPointCoord.x = cameraPt.x - size.width / 2;
    midPointCoord.y = -cameraPt.y + size.height/ 2;
    return midPointCoord;
}

}
