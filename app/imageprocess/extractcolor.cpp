#include "extractcolor.h"
#include "opencv_utils.h"

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
    // Create lower and upper color bounds
    cv::Scalar colorToFilter= QColor2Scalar(_colorToFilter);
    int tolerance = 20;
    cv::Scalar colorLower(colorToFilter[0] - tolerance, 80,80); // h, s, v
    cv::Scalar colorUpper(colorToFilter[0] + tolerance, 255,255);

    // Convet input image to hsv space
    cv::Mat imgHSV;
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

    cv::Mat imgThresholded;
    cv::inRange(imgHSV, colorLower, colorUpper, imgThresholded);
    //morphological opening (remove small objects from the foreground)
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::dilate( imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

     //morphological closing (fill small holes in the foreground)
    cv::dilate( imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );


    // Set up the detector with default parameters.
    // Setup SimpleBlobDetector parameters.
    cv::SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 255;
    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 100;
    params.maxArea = 50000;

    // Filter by Circularity
    params.filterByCircularity = false;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = false;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = false;
    params.minInertiaRatio = 0.01;
    auto detector = cv::SimpleBlobDetector::create(params);
    cv::threshold (imgThresholded, imgThresholded, 70, 255, CV_THRESH_BINARY_INV);

    // Detect blobs.
    std::vector<cv::KeyPoint> keyPoints;
    detector->detect( imgThresholded, keyPoints );

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::Mat imgWithKeypoints;
    cv::drawKeypoints( imgThresholded, keyPoints, imgWithKeypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    cv::imshow("Thresholded Frame", imgWithKeypoints); // Show output image

    auto largestKeyPoint = cv_utils::GetLargestKeyPoint(keyPoints);

    auto cameraSize = imgWithKeypoints.size();
    auto midPtCoord = cv_utils::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
// In pixels
    std::cout << midPtCoord.x << " " << midPtCoord.y << " " << largestKeyPoint.size << std::endl;

//    Distance distance;
//    emit NewDistance(distance);
}






