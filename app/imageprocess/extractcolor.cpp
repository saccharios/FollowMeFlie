#include "extractcolor.h"


cv::Scalar QColor2Scalar(QColor const & color)
{
    int h = 0;
    int s = 0;
    int v = 0;;
    color.getHsv(&h, &s, &v);
    return cv::Scalar(h/2,s,v); // In opencv, hue ranges from 0 to 179
}


void ExtractColor::ProcessImage(cv::Mat const & img)
{
    // Convet input image to hsv space
    cv::Mat imgHSV;
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
    // Create lower and upper color bounds
    cv::Scalar colorToFilter= QColor2Scalar(_colorToFilter);
    int tolerance = 20;
    cv::Scalar colorLower(colorToFilter[0] - tolerance, 120,120);
    cv::Scalar colorUpper(colorToFilter[0] + tolerance, 255,255);
    // TODO SF:: Special case red!
    cv::Mat imgThreshold;
    cv::inRange(imgHSV, colorLower, colorUpper, imgThreshold);

    cv::imshow("Thresholded Frame", imgThreshold); // Show camera stream in separate window

//    Distance distance;
//    emit NewDistance(distance);
}






