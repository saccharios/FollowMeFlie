#include "extract_color.h"
#include "opencv_utils.h"
#include "math/constants.h"
#include "math/types.h"
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
    Blobs blobs = ExtractKeyPoints(img, imgToShow);

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    cv::drawKeypoints( imgToShow, blobs.camPoints, imgToShow, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );



    cv::KeyPoint estimateMidPtCoord = _kalmanFilter.Update(blobs.midPoints);
    std::cout << "estimateMidPtCoord size = " << estimateMidPtCoord.size << std::endl;

    float distance = CalculateDistance(estimateMidPtCoord);
    std::cout << "distance = " << distance << std::endl;

    // Draw the estimate
    cv::Point2f estimateCamera = Camera::ConvertMidPointToCameraCoord(estimateMidPtCoord.pt);
    cv::circle(imgToShow, estimateCamera, 25, {230,250,25},3);
    // Draw circle in the middle
    cv::circle(imgToShow, Camera::GetMidPoint(), 25, {200,10,50}, 3);

    cv::imshow("Thresholded Frame", imgToShow); // Show output image

    Distance positionEstimate = {estimateMidPtCoord.pt.x, estimateMidPtCoord.pt.y, distance};
    emit EstimateReady(positionEstimate);

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

ExtractColor::Blobs ExtractColor::ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgToShow)
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
    Blobs blobs;

    auto detector = cv::SimpleBlobDetector::create(_detectorParams);
    detector->detect( imgToShow, blobs.camPoints );

    AddMidPtCoord(blobs);


    // Remove keypoints close to the edge
    RemoveKeyPointsAtEdges(blobs);

    return blobs;
}

void ExtractColor::AddMidPtCoord(Blobs & blobs)
{
    blobs.midPoints = blobs.camPoints;
    for(uint8_t idx = 0; idx < blobs.camPoints.size(); ++idx)
    {
        blobs.midPoints.at(idx).pt = Camera::ConvertCameraToMidPointCoord(blobs.camPoints.at(idx).pt);
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

float ExtractColor::CalculateDistance(cv::KeyPoint const & point)
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
    float a = 0.0007058662;
    float b = -0.0008086623;

    return 1/(a*point.size + b);
}

void ExtractColor::Initialize(cv::Mat const & img)
{
    cv::Mat imgWithKeypoints;
    Blobs blobs = ExtractKeyPoints(img, imgWithKeypoints);

    auto largestKeyPoint = opencv_utils::GetLargestKeyPoint(blobs.camPoints);

    // Kalman filter
//    auto midPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt, cameraSize);
//    std::cout << "Measurement = "<< midPtCoord.x << " " << midPtCoord.y << std::endl;

    auto measurementMidPtCoord = Camera::ConvertCameraToMidPointCoord(largestKeyPoint.pt);
    _kalmanFilter.Initialize(measurementMidPtCoord);

}

void ExtractColor::RemoveKeyPointsAtEdges(Blobs & blobs)
{
    // Receives keyPoints in midPoint Coordinates.
    // Removes all keyPoints that are further away from the midPoint than (x-resultion/2)*0.9;
    float limit = (Camera::GetResolution().width/2.0 *0.9);
    float limit_sqr = limit*limit;

    for(uint8_t idx = 0; idx < blobs.midPoints.size(); )
    {

        if( (blobs.midPoints.at(idx).pt.x * blobs.midPoints.at(idx).pt.x) +
                (blobs.midPoints.at(idx).pt.y * blobs.midPoints.at(idx).pt.y)
                > (limit_sqr))
        {
            blobs.camPoints.erase(blobs.camPoints.begin() + idx);
            blobs.midPoints.erase(blobs.midPoints.begin() + idx);
        }
        else
        {
            ++idx;
        }
    }
}

