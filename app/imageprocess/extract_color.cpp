#include "extract_color.h"
#include "opencv_utils.h"
#include "math/constants.h"
#include "math/types.h"
#include "text_logger.h"


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
    cv::Mat imgToShow;
    std::vector<cv::KeyPoint> camPoints= ExtractKeyPoints(img, imgToShow);

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints( imgToShow, camPoints, imgToShow, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    std::vector<Blob> crazyFliePoints = Camera::ConvertCameraToCrazyFlieCoord(camPoints);

    Blob estimateCrazyFlieCoord = _kalmanFilter.Update(crazyFliePoints);

    // Draw the estimate
    cv::Point2f estimateCamera = Camera::ConvertCrazyFlieToCameraCoord(estimateCrazyFlieCoord);
    cv::circle(imgToShow, estimateCamera, 25, {230,250,25},3);
    // Draw circle in the middle
    cv::circle(imgToShow, Camera::GetMidPoint(), 25, {200,10,50}, 3);

    cv::imshow("Thresholded Frame", imgToShow); // Show output image

    textLogger << "Camer estimatea: x (m) " << estimateCrazyFlieCoord.point.x
               << " y (m) = " << estimateCrazyFlieCoord.point.y
               << " z (m) = " << estimateCrazyFlieCoord.point.z << "\n";

    emit EstimateReady(estimateCrazyFlieCoord.point);

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

std::vector<cv::KeyPoint> ExtractColor::ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgToShow)
{
    // Create lower and upper color bounds
    cv::Scalar colorToFilter= QColor2Scalar(_colorToFilter);
    int tolerance = 20; // Arbitrary value but seems to be good.
    cv::Scalar colorLower(colorToFilter[0] - tolerance, 80,80); // h, s, v
    cv::Scalar colorUpper(colorToFilter[0] + tolerance, 255,255);

    cv::Mat imgHSV;
    ConvertToHSV(img, imgHSV, colorLower, colorUpper);

    // Filter by color
    cv::inRange(imgHSV, colorLower, colorUpper, imgToShow);

    // Filter holes away
    FilterImage(imgToShow);

    // Create black/white picture
    cv::threshold (imgToShow, imgToShow, 70, 255, CV_THRESH_BINARY_INV);

    // Detect blobs.
    std::vector<cv::KeyPoint> cameraKeyPoints;

    auto detector = cv::SimpleBlobDetector::create(_detectorParams);
    detector->detect( imgToShow, cameraKeyPoints );

    // Remove keypoints close to the edge
    RemoveKeyPointsAtEdges(cameraKeyPoints);

    return cameraKeyPoints;
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

void ExtractColor::Initialize(cv::Mat const & img)
{
    cv::Mat imgWithKeypoints;
    std::vector<cv::KeyPoint> camPoints = ExtractKeyPoints(img, imgWithKeypoints);
    std::vector<Blob> crazyFliePoints = Camera::ConvertCameraToCrazyFlieCoord(camPoints);

    auto largestBlob = GetLargestBlob(crazyFliePoints);

    // Kalman filter
    _kalmanFilter.Initialize(largestBlob);

    textLogger << "Measurement cfly = "<< largestBlob.point.x << " " << largestBlob.point.y << " " << largestBlob.point.z << "\n";
}

void ExtractColor::RemoveKeyPointsAtEdges(std::vector<cv::KeyPoint> & cameraKeyPoints)
{
    // Receives keyPoints in midPoint Coordinates.
    // Removes all keyPoints that are further away from the midPoint than (x-resultion/2)*0.9;
    float limit = (Camera::GetResolution().width/2.0 *0.9);
    float limit_sqr = limit*limit;

    for(uint8_t idx = 0; idx < cameraKeyPoints.size(); )
    {
        cv::Point2f midPoint = Camera::ConvertCameraToMidPointCoord(cameraKeyPoints.at(idx).pt);

        if( (midPoint.x * midPoint.x) + (midPoint.y * midPoint.y)
                > (limit_sqr))
        {
            cameraKeyPoints.erase(cameraKeyPoints.begin() + idx);
        }
        else
        {
            ++idx;
        }
    }
}

