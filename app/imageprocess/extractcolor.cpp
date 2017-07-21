#include "extractcolor.h"


cv::Scalar QColor2Scalar(QColor const & color)
{
    int h = 0;
    int s = 0;
    int v = 0;;
    color.getHsv(&h, &s, &v);
    return cv::Scalar(h,s,v);
}


void ExtractColor::ProcessImage(cv::Mat const & img)
{
    // Convet input image to hsv space
    cv::Mat imgHSV;
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
    // Create lower and upper color bounds
    cv::Scalar colorToFilter= QColor2Scalar(_colorToFilter);
    cv::Scalar tolerance(10,10,10);
    cv::Scalar colorLower = colorToFilter - tolerance;
    cv::Scalar colorUppder = colorToFilter + tolerance;
    // TODO SF:: Special case red!
    cv::Mat imgThreshold;
    cv::inRange(imgHSV, colorLower, colorUppder, imgThreshold);

    cv::imshow("Thresholded Frame", imgThreshold); // Show camera stream in separate window

//    Distance distance;
//    emit NewDistance(distance);
}






