#include "camera.h"
#include <memory>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <QImage>
#include <QCameraInfo>
#include "text_logger.h"
#include "math/types.h"

QImage Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copy
     QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     dest.bits(); // enforce deep copy
     return dest;
}

Camera::Camera () :
     _state(CameraState::DISABLED),
     _activated(false),
     _capture(new cv::VideoCapture)
{
}
cv::Size Camera::_resolution = cv::Size();
cv::Point2f Camera::_midPoint = cv::Point2f();

void Camera::Update()
{
    switch(_state)
    {
    default:
    case CameraState::DISABLED:
    {
        if(_activated)
        {
            _state = CameraState::CONNECTING;
        }

        break;
    }
    case CameraState::CONNECTING:
    {
        textLogger << "Found Cameras: " << QCameraInfo::availableCameras().count() << "\n";
        _capture->open(1); // 0 for laptop camera // 1 for crazyflie camera // 2 for creative camera
        if(_activated && _capture->isOpened())
        {
            _state = CameraState::RUNNING;
            _resolution.width = _capture->get(CV_CAP_PROP_FRAME_WIDTH);
            _resolution.height = _capture->get(CV_CAP_PROP_FRAME_HEIGHT);
            _midPoint = ConvertMidPointToCameraCoord({0.0, 0.0});

            _capture->set(CV_CAP_FFMPEG,true);
            _capture->set(CV_CAP_PROP_FPS,30);


            textLogger << "Camera ok. Resolution is " << _resolution.width << " x " << _resolution.height << "\n";
            textLogger << "Settings: \n";
            textLogger << "CV_CAP_PROP_POS_MSEC  = " << _capture->get(CV_CAP_PROP_POS_MSEC ) << "\n";
            textLogger << "CV_CAP_PROP_POS_FRAMES   = " << _capture->get(CV_CAP_PROP_POS_FRAMES  )<< "\n";
            textLogger << "CV_CAP_PROP_POS_AVI_RATIO  = " << _capture->get(CV_CAP_PROP_POS_AVI_RATIO )<< "\n";
            textLogger << "CV_CAP_PROP_FPS  = " << _capture->get(CV_CAP_PROP_FPS)  << "\n";
            textLogger << "CV_CAP_PROP_FOURCC  = " << _capture->get(CV_CAP_PROP_FOURCC)  << "\n";
            textLogger << "CV_CAP_PROP_FRAME_COUNT  = " << _capture->get(CV_CAP_PROP_FRAME_COUNT)  << "\n";
            textLogger << "CV_CAP_PROP_FORMAT  = " << _capture->get(CV_CAP_PROP_FORMAT )<< "\n";
            textLogger << "CV_CAP_PROP_MODE  = " << _capture->get(CV_CAP_PROP_MODE)  << "\n";
            textLogger << "CV_CAP_PROP_BRIGHTNESS  = " << _capture->get(CV_CAP_PROP_BRIGHTNESS ) << "\n";
            textLogger << "CV_CAP_PROP_CONTRAST  = " << _capture->get(CV_CAP_PROP_CONTRAST ) << "\n";
            textLogger << "CV_CAP_PROP_SATURATION   = " << _capture->get(CV_CAP_PROP_SATURATION  ) << "\n";
            textLogger << "CV_CAP_PROP_HUE   = " << _capture->get(CV_CAP_PROP_HUE  )<< "\n";
            textLogger << "CV_CAP_PROP_GAIN   = " << _capture->get(CV_CAP_PROP_GAIN  )<< "\n";
            textLogger << "CV_CAP_PROP_EXPOSURE   = " << _capture->get(CV_CAP_PROP_EXPOSURE  )<< "\n";
            textLogger << "CV_CAP_PROP_CONVERT_RGB   = " << _capture->get(CV_CAP_PROP_CONVERT_RGB  )<< "\n";
            textLogger << "CV_CAP_PROP_CONTRAST  = " << _capture->get(CV_CAP_PROP_CONTRAST)  << "\n";
            InitializeTracking();
        }
        else if(!_activated)
        {
            _capture->release();
            _state = CameraState::DISABLED;
        }
        else if(_activated)
        {
            std::cout << "Failed to open camera\n";
            textLogger << "Failed to open camera\n";
        }
        break;
    }
    case CameraState::RUNNING:
    {
        FetchAndImageReady();
        if ( !_activated)
        {
            _capture->release();
            _state = CameraState::DISABLED;
        }
        break;
    }
    }
}



void Camera::Activate(bool activate)
{
    _activated = activate;
}




void Camera::FetchAndImageReady()
{
    cv::Mat frame;
    FetchImage(frame);
    //    cv::imshow("Original Frame", (frame)); // Show camera stream in separate window
    auto image = Mat2QImage(frame);
    emit ImgReadyForDisplay(image);
    emit ImgReadyForProcessing(frame);
}

void Camera::FetchImage(cv::Mat & frame)
{
    (*_capture) >> frame;
    if( (frame).empty() )
    {
        textLogger << "ERROR!! Failed to capture frame\n";
        return;
    }
}

void Camera::InitializeTracking()
{
    cv::Mat frame;
    FetchImage(frame);
    FetchImage(frame); // Fetch twice, as the first image contains nothing
    emit ImgReadyForInitialization(frame);
}

std::vector<Blob> Camera::ConvertCameraToCrazyFlieCoord(std::vector<cv::KeyPoint> const & keyPoints)
{
    std::vector<Blob> blobs;
    for(auto const & keyPoint : keyPoints)
    {
        blobs.push_back(ConvertCameraToCrazyFlieCoord(keyPoint));
    }
    return blobs;
}

Blob Camera::ConvertCameraToCrazyFlieCoord(cv::KeyPoint cameraPt)
{
    // Camera coordinate system is, (0,0) is in the top left corner
    // positive x to the right (in pixel)
    // positive y to the bottowm (in pixel)

    // Crazyflie coordinate system is, (0,0,0) is at the crazyflie
    // 1)The x-direction is positive in the front direction (antenna) (in meter)
    // 2)The y-direction is positive leftward (in meter)
    // 3)The z-direction is positive upward (in meter)

    // TODO SF, simplify
    cv::Point2f midPtCoord = ConvertCameraToMidPointCoord(cameraPt.pt);

    cv::KeyPoint kPt = cameraPt;
    kPt.pt = midPtCoord;
    float distance = EstimateDistance(kPt);

    Point3f point;
    point.x = midPtCoord.x;
    point.y = midPtCoord.y;
    point.z = distance;

    Blob blob;
    blob.point = ConvertMidPointToCrazyFlieCoordinates(point);
    blob.size = cameraPt.size;
    return blob;
}
cv::Point2f  Camera::ConvertCameraToMidPointCoord(cv::Point2f cameraPt)
{
    cv::Point2f midPointCoord;
    midPointCoord.x = cameraPt.x - _resolution.width / 2;
    midPointCoord.y = -cameraPt.y + _resolution.height/ 2;
    return midPointCoord;
}

cv::Point2f  Camera::ConvertMidPointToCameraCoord(cv::Point2f midPt )
{
    cv::Point2f cameraCoord;
    cameraCoord.x = midPt.x + _resolution.width/2;
    cameraCoord.y = -midPt.y + _resolution.height/2;
    return cameraCoord;
}
float pixelToMeter()
{
    // TODO SF take into account depth (camera z) to calculate x,y conversion to meter
    return 1.0;
}
float MeterToPixel()
{
    // TODO SF take into account depth (camera z) to calculate x,y conversion to meter
    return 1/pixelToMeter();
}

Point3f Camera::ConvertMidPointToCrazyFlieCoordinates(Point3f pointCamera)
{
    // MidPt coordinate system is looking for the camera:
    // positive x to the right (in pixel)
    // positive y to the top (in pixel)
    // positive z into the image (in cm)

    // Crazyflie coordinate system is:
    // 1)The x-direction is positive in the front direction (antenna) (in meter)
    // 2)The y-direction is positive leftward (in meter)
    // 3)The z-direction is positive upward (in meter)

    Point3f pointCFly;
    pointCFly.x = pointCamera.z/100.0;
    pointCFly.y = -pointCamera.x*pixelToMeter();
    pointCFly.z = pointCamera.y*pixelToMeter();
    return pointCFly;
}

Point3f Camera::ConvertCrazyFlieToMidPointCoord(Blob blob)
{

    Point3f midPt;
    midPt.x = -blob.point.y*MeterToPixel();
    midPt.y = blob.point.z*MeterToPixel();
    midPt.z = blob.point.x * 100.0;
    return midPt;
}


cv::Point2f Camera::ConvertCrazyFlieToCameraCoord(Blob blob)
{
    Point3f midPt = ConvertCrazyFlieToMidPointCoord(blob);
    return ConvertMidPointToCameraCoord({midPt.x, midPt.y});
}


float Camera::EstimateDistance(cv::KeyPoint const & point)
{
    // Use a primitive way of estimating the distance. This is known to be incorrect at the edges and the corners
    // of the camera if the ball is far away (due to fish eye). But then it anyways does not matter.
    // Formula is
    // Distance = 1/(a * size +b)
    // a and b are calculated with experimental values
    // distance | size
    //       10 | 160
    //       16 | 92
    //       26 | 57
    //       51 | 27
    float a = 0.0007058662;
    float b = -0.0008086623;
    float distance = 1.0/(a*point.size + b);
    if(distance < 0.0)
    {
        distance = 0.0;
    }
    return distance;
}
