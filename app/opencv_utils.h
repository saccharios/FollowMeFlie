#include "opencv2/opencv.hpp"

#pragma once

namespace opencv_utils
{

cv::KeyPoint GetLargestKeyPoint(std::vector<cv::KeyPoint> const & keypoints);

}
