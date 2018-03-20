#include "extract_color.h"
#include "opencv_utils.h"
#include "math/constants.h"

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
    cv::Mat imgWithKeypoints;
    std::vector<cv::KeyPoint> keyPointsCamCoord = ExtractKeyPoints(img, imgWithKeypoints);

    std::vector<cv::KeyPoint> keyPointsMidPtCoord = Camera::ConvertCameraToMidPointCoord(keyPointsCamCoord);

    cv::KeyPoint estimateMidPtCoord = _kalmanFilter.Update(keyPointsMidPtCoord);
    std::cout << "estimateMidPtCoord size = " << estimateMidPtCoord.size << std::endl;

    double distance = CalculateDistance(estimateMidPtCoord);
    std::cout << "distance = " << distance << std::endl;

    // Draw the estimate
    cv::Point estimateCamera = Camera::ConvertMidPointToCameraCoord(estimateMidPtCoord.pt);
    cv::circle(imgWithKeypoints, estimateCamera, 30, {230,250,25},3);
    cv::imshow("Thresholded Frame", imgWithKeypoints); // Show output image
}

void ExtractColor::ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper)
{
    // Convert input image to hsv space
    //Special case if we want to detect some red-ish color
    if(colorLower[0] < 0 )
    {
        //Invert image and look for cyan (is at 90°)
        cv::cvtColor(~img, imgHSV, cv::COLOR_BGR2HSV);
        cv::imshow("invertec picture", ~img); // Show output image
        colorLower[0] += 90;
        colorUpper[0] += 90;
    }
    else if(colorUpper[0] > 180)
    {
        //Invert image and look for cyan (is at 90°)
        cv::cvtColor(~img, imgHSV, cv::COLOR_BGR2HSV);
        cv::imshow("invertec picture", ~img); // Show output image
        colorLower[0] -= 90;
        colorUpper[0] -= 90;
    }
    else
    { // Normal case
        cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
    }
}

std::vector<cv::KeyPoint> ExtractColor::ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgWithKeypoints)
{
    // Create lower and upper color bounds
    cv::Scalar colorToFilter= QColor2Scalar(_colorToFilter);
    int tolerance = 20; // Arbitrary value but seems to be good.
    cv::Scalar colorLower(colorToFilter[0] - tolerance, 80,80); // h, s, v
    cv::Scalar colorUpper(colorToFilter[0] + tolerance, 255,255);

    cv::Mat imgHSV;
    ConvertToHSV(img, imgHSV, colorLower, colorUpper);

    // Filter by color
    cv::Mat imgThresholded;
    cv::inRange(imgHSV, colorLower, colorUpper, imgThresholded);

    // Filter holes away
    FilterImage(imgThresholded);

    // Create black/white picture
    cv::threshold (imgThresholded, imgThresholded, 70, 255, CV_THRESH_BINARY_INV);

    // Detect blobs.
    std::vector<cv::KeyPoint> keyPoints;

    auto detector = cv::SimpleBlobDetector::create(_detectorParams);
    detector->detect( imgThresholded, keyPoints );


    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints( imgThresholded, keyPoints, imgWithKeypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    return keyPoints;
}



void ExtractColor::FilterImage(cv::Mat & imgThresholded)
{
    //morphological opening (remove small objects from the foreground)
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::dilate( imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

    //morphological closing (fill small holes in the foreground)
    cv::dilate( imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
}

double ExtractColor::CalculateDistance(cv::KeyPoint const & point)
{
    // Use a primitive way of estimating the distance. This is known to be incorrect at the edges and the corners
    // of the camera if the ball is far away (due to fish eye). But then it anyways does not matter.
    // Formula is
    // Distance = 1/(a * size +b)
    // a and b are calculated with experimental values
    // d | s
    // 10 | 160
    // 16 | 92
    // 26 | 57
    // 51 | 27
    double a = 0.0007058662;
    double b = -0.0008086623;

    return 1/(a*point.size + b);
}

void ExtractColor::Initialize(cv::Mat const & img)
{
    cv::Mat imgWithKeypoints;
    auto keyPoints = ExtractKeyPoints(img, imgWithKeypoints);

    auto largestKeyPoint = opencv_utils::GetLargestKeyPoint(keyPoints);

    // Kalman filter
//    auto midPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
//    std::cout << "Measurement = "<< midPtCoord.x << " " << midPtCoord.y << std::endl;

    auto measurementMidPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt);
    _kalmanFilter.Initialize(measurementMidPtCoord);

}

