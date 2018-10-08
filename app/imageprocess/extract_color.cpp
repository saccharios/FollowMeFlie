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
    std::vector<cv::KeyPoint> camPoints = ExtractKeyPoints(img, imgToShow);

    // Draw detected keyPoints as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of keyPoint
    cv::drawKeypoints( imgToShow, camPoints, imgToShow, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    Point3f estimateBallCoordinatesFromCFlie_2d = ProcessWithKalman2d(camPoints);
    Point3f estimateBallCoordinatesFromCFlie_3d = ProcessWithKalman3d(camPoints);

    emit EstimateReady(estimateBallCoordinatesFromCFlie_2d);

    DrawEstimate(imgToShow, estimateBallCoordinatesFromCFlie_2d, {255,255,0}); // Cyan
    DrawEstimate(imgToShow, estimateBallCoordinatesFromCFlie_3d, {0,255,255} ); // Yellow
    DrawEstimate(imgToShow, _setPoint, {255,0,0} ); // Blue

    cv::imshow("Thresholded Frame", imgToShow); // Show output image
}

void ExtractColor::DrawEstimate(cv::Mat & imgToShow, Point3f pointCrazyFlieCoord, cv::Scalar color)
{
    // Draw the estimate
    cv::KeyPoint point = Camera::ConvertCrazyFlieCoordToCameraCoord(pointCrazyFlieCoord);
    int radius = point.size * _factor;
    if(radius <= 1)
    {
        radius = 1;
    }
    cv::circle(imgToShow, point.pt, radius, color, 2);
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
    int tolerance = 25; // Arbitrary value but seems to be good.
    cv::Scalar colorLower(colorToFilter[0] - tolerance, 80,80); // h, s, v
    cv::Scalar colorUpper(colorToFilter[0] + tolerance, 255,255);

    cv::Mat imgHSV;
    ConvertToHSV(img, imgHSV, colorLower, colorUpper);

    // Filter by color
    cv::inRange(imgHSV, colorLower, colorUpper, imgToShow);

    // Filter holes away
    FilterImage(imgToShow);

    // Create black/white picture
    cv::threshold (imgToShow, imgToShow, 60, 255, CV_THRESH_BINARY_INV);

    // Detect keyPoints
    std::vector<cv::KeyPoint> cameraKeyPoints;
    auto detector = cv::SimpleBlobDetector::create(_detectorParams);
    detector->detect( imgToShow, cameraKeyPoints );

    // Remove keypoints close to the edge
    RemoveKeyPointsAtEdges(cameraKeyPoints);

    return cameraKeyPoints;
}


void ExtractColor::FilterImage(cv::Mat & imgThresholded)
{
    int erosion_size = 2;
    //morphological opening (remove small objects from the foreground)
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point( erosion_size, erosion_size  ) ));
    cv::dilate(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point( erosion_size, erosion_size  ) ));

    //morphological closing (fill small holes in the foreground)
    cv::dilate(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point( erosion_size, erosion_size  ) ));
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point( erosion_size, erosion_size  ) ));
}

void ExtractColor::Initialize(cv::Mat const & img)
{
    cv::Mat imgWithKeypoints;
    std::vector<cv::KeyPoint> camPoints = ExtractKeyPoints(img, imgWithKeypoints);
    std::vector<MidPoint> midPoints = Camera::ConvertCameraToMidPointCoord(camPoints);

    MidPoint largestMidPoint = GetLargest(midPoints);

    // Kalman filter
    _kalmanFilter_2d.Initialize(largestMidPoint);

    _kalmanFilter_3d.Initialize(Camera::ConvertMidPointToCrazyFlieCoord(largestMidPoint));

//    textLogger << "Measurement cfly = "<< largestMidPoint.pt.x << " " << largestMidPoint.pt.y << " " << largestMidPoint.size << "\n";
}

void ExtractColor::RemoveKeyPointsAtEdges(std::vector<cv::KeyPoint> & cameraKeyPoints)
{
    // Receives keyPoints in midPoint Coordinates.
    // Removes all keyPoints that are further away from the midPoint than (x-resultion/2)*0.9;
    float limit = (Camera::GetResolution().width/2.0 *0.9);
    float limit_sqr = limit*limit;

    for(uint8_t idx = 0; idx < cameraKeyPoints.size(); )
    {
        MidPoint midPoint = Camera::ConvertCameraToMidPointCoord(cameraKeyPoints.at(idx));

        if( (midPoint.pt.x * midPoint.pt.x) + (midPoint.pt.y * midPoint.pt.y)
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

Point3f ExtractColor::ProcessWithKalman2d(std::vector<cv::KeyPoint> const & camPoints)
{
    std::vector<MidPoint> midPointMeasurements = Camera::ConvertCameraToMidPointCoord(camPoints);
    MidPoint estimate = _kalmanFilter_2d.Update(midPointMeasurements);

//    std::cout << "MidPoint estimate: size = " << estimate.size
//               << " x (pixel) = " << estimate.pt.x
//               << " y (pixel) = " << estimate.pt.y<< "\n";

    Point3f estimateBallCoordinatesFromCFlie = Camera::ConvertMidPointToCrazyFlieCoord(estimate);
//    std::cout << "estimateBallCoordinatesFromCFlie: x (m) = " << estimateBallCoordinatesFromCFlie.x
//              << " y (m) = " << estimateBallCoordinatesFromCFlie.y
//              << " z (m) = " << estimateBallCoordinatesFromCFlie.z << "\n";
    return estimateBallCoordinatesFromCFlie;
}
Point3f ExtractColor::ProcessWithKalman3d(std::vector<cv::KeyPoint> const & camPoints)
{
    std::vector<Point3f> crazyFliePointMeasurements = Camera::ConvertCameraToCrazyFlieCoord(camPoints);

    BallKalmanFilter_3d::Vector6f estimate = _kalmanFilter_3d.Update(crazyFliePointMeasurements);


//    std::cout << "estimate: x (m) = " << estimate[0]
//              << " y (m) = " << estimate[1]
//              << " z (m) = " << estimate[2]
//              << " vx (m/s) = " << estimate[3]
//              << " vy (m/s) = " << estimate[4]
//              << " vz (m/s) = " << estimate[5]
//              << "\n";
    Point3f output;
    output.x = estimate[0];
    output.y = estimate[1];
    output.z = estimate[2];

    return output;
}
