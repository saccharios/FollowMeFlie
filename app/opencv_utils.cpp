#include "opencv_utils.h"

namespace opencv_utils
{

cv::KeyPoint GetLargestKeyPoint(std::vector<cv::KeyPoint> const & keypoints)
{
    cv::KeyPoint largestKeyPoint(0,0,0);
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

}
