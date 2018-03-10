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
    // TODO SF No need to always create parameters a new!
    auto params = CreateParameters();
    auto detector = cv::SimpleBlobDetector::create(params);
    std::vector<cv::KeyPoint> keyPoints;
    detector->detect( imgThresholded, keyPoints );

    cv::Mat imgWithKeypoints;
    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints( imgThresholded, keyPoints, imgWithKeypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    auto largestKeyPoint = cv_utils::GetLargestKeyPoint(keyPoints);

    cv::Size cameraSize = imgThresholded.size(); // 360, 640 default resolution

    // TODO SF No need to always create parameters a new! These settings depend on the camera!
    double blobSizeToLength = 1/287.0; // Factor to convert blob size to mm, experimental value
    double focalLength = 1.92; // Focal length of camera in mm
    double sizeBall = 68; // Diameter of the tennis ball in mm
    double fieldOfVview = 66*pi/180.0; // 66° diagonal

    // Kalman filter
    auto midPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
    std::cout << "Measurement = "<< midPtCoord.x << " " << midPtCoord.y << std::endl;
    cv::Point2f estimateMidPtCoord= Filter(largestKeyPoint, cameraSize);
//    std::cout << "estimate = " << estimateMidPtCoord.x << ", " << estimateMidPtCoord.y << "\n";

    auto distance = CalculateDistance(estimateMidPtCoord,
                                      largestKeyPoint.size,
                                      cameraSize,
                                      blobSizeToLength,
                                      focalLength,
                                      sizeBall,
                                      fieldOfVview);
    //    std::cout << distance.x << " " << distance.y << " " << distance.z << std::endl;

    // Run Kalman filter on the d

    cv::Point estimateCamera = Camera::ConvertMidPointToCameraCoord(estimateMidPtCoord, cameraSize);
    cv::circle(imgWithKeypoints, estimateCamera, 30, {230,250,25},3);

    //    std::cout << "_state_estimation = " << center.x << " "
    //                 << center.y << " "
    //                    << state_estimate[2] << " "
    //                       << state_estimate[3] << "\n";
    //    std::cout << "---------------------------------------------\n";

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

void ExtractColor::FilterImage(cv::Mat & imgThresholded)
{
    //morphological opening (remove small objects from the foreground)
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::dilate( imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );

    //morphological closing (fill small holes in the foreground)
    cv::dilate( imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
    cv::erode(imgThresholded, imgThresholded, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)) );
}

cv::SimpleBlobDetector::Params ExtractColor::CreateParameters()
{
    // Set up the detector with default parameters.
    // Setup SimpleBlobDetector parameters.
    cv::SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 255;
    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 200;
    params.maxArea = 100000;

    // Filter by Circularity
    params.filterByCircularity = false;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = false;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = false;
    params.minInertiaRatio = 0.01;

    return params;
}


cv::Point2f ExtractColor::Filter(cv::KeyPoint const & largestKeyPoint, cv::Size cameraSize)
{
    static cv::Point2f pointFilteredOld = {0,0};
    if(largestKeyPoint.size == 0)
    {
        // If we don't have a measurement, we return the old filtered point
        // TODO SF Better: Let the kalman filter run, but with different matrices
        return pointFilteredOld;
    }
    auto measurementMidPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
    Eigen::Vector4f state_estimate  = _kalmanFilter.Update(measurementMidPtCoord);
    cv::Point2f pointFiltered = cv::Point2f{state_estimate[0], state_estimate[1]};
    pointFilteredOld = pointFiltered;
    return pointFiltered;
}

Distance ExtractColor::CalculateDistance(cv::Point2f point,
                                         double size,
                                         cv::Size cameraSize,
                                         double blobSizeToLength,
                                         double focalLength,
                                         double sizeBall,
                                         double fieldOfView)
{
    double resolutionRatio = cameraSize.width / cameraSize.height;
    // Estimate depth
    double depth = focalLength *sizeBall /(blobSizeToLength * size);

    // TODO SF Can be optimized !
    double totalWidth = 2.0*depth*sin(fieldOfView / 2.0) / sqrt(1.0+1.0/(resolutionRatio*resolutionRatio));
    double totalHeight = totalWidth / resolutionRatio;
    double widthToLength = totalWidth / cameraSize.width;
    double heightToLength= totalHeight / cameraSize.height;

    Distance distance; // in mm
    distance.x = point.x * widthToLength;
    distance.y = point.y * heightToLength;
    distance.z = depth;
    return distance;
}

void ExtractColor::Initialize(cv::Mat const & img)
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
    // TODO SF No need to always create parameters a new!
    auto params = CreateParameters();
    auto detector = cv::SimpleBlobDetector::create(params);
    std::vector<cv::KeyPoint> keyPoints;
    detector->detect( imgThresholded, keyPoints );

    cv::Mat imgWithKeypoints;
    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints( imgThresholded, keyPoints, imgWithKeypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    auto largestKeyPoint = cv_utils::GetLargestKeyPoint(keyPoints);

    cv::Size cameraSize = imgThresholded.size(); // 360, 640 default resolution


    // Kalman filter
//    auto midPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
//    std::cout << "Measurement = "<< midPtCoord.x << " " << midPtCoord.y << std::endl;

    auto measurementMidPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
    _kalmanFilter.Initialize(measurementMidPtCoord);

}

