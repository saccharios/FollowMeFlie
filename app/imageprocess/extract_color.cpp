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
    auto params = CreateParameters();
    auto detector = cv::SimpleBlobDetector::create(params);
    std::vector<cv::KeyPoint> keyPoints;
    detector->detect( imgThresholded, keyPoints );

    DrawBlobs(imgThresholded, keyPoints);

    auto largestKeyPoint = cv_utils::GetLargestKeyPoint(keyPoints);

    cv::Size cameraSize = imgThresholded.size(); // 360, 640 default resolution

    double blob_size_to_length = 1/287.0; // Factor to convert blob size to mm, experimental value
    double focal_length = 1.92; // Focal length of camera in mm
    double size_ball = 68; // Diameter of the tennis ball in mm
    double field_of_view = 66*pi/180.0; // 66° diagonal
    auto distance = CalculateDistance(largestKeyPoint,
                                      cameraSize,
                                      blob_size_to_length,
                                      focal_length,
                                      size_ball,
                                      field_of_view);
    std::cout << distance.x << " " << distance.y << " " << distance.z << std::endl;

    // Run Kalman filter on the distance
}

void ExtractColor::ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper)
{
    // Convet input image to hsv space
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
    params.minArea = 100;
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

void ExtractColor::DrawBlobs(cv::Mat const & img, std::vector<cv::KeyPoint> const & keyPoints)
{
    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::Mat imgWithKeypoints;
    cv::drawKeypoints( img, keyPoints, imgWithKeypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    cv::imshow("Thresholded Frame", imgWithKeypoints); // Show output image
}

Distance ExtractColor::CalculateDistance(cv::KeyPoint const & largestKeyPoint,
                                                             cv::Size cameraSize,
                                                             double blob_size_to_length,
                                                             double focal_length,
                                                             double size_ball,
                                                             double field_of_view)
{
    static Distance distance_old = {0,0,0};
    // If we don't have a valid measurement we return the previous distance measurement.
    if(largestKeyPoint.size == 0)
    {
        return distance_old;
    }
    double resolution_ratio = cameraSize.width / cameraSize.height;
    auto midPtCoord = cv_utils::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);

    // Estimate depth
    double depth = focal_length *size_ball /(blob_size_to_length * largestKeyPoint.size);


    double total_width = 2*depth*sin(field_of_view / 2.0) / sqrt(1+1/(resolution_ratio*resolution_ratio));
    double total_height = total_width / resolution_ratio;
    double width_to_length = total_width / cameraSize.width;
    double height_to_length= total_height / cameraSize.height;

    Distance distance; // in mm
    distance.x = midPtCoord.x * width_to_length;
    distance.y = midPtCoord.y * height_to_length;
    distance.z = depth;
    distance_old = distance;
    return distance;
}
