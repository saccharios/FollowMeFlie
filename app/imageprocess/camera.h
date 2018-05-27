#pragma once
#include <QObject>
#include<memory>
#include "opencv2/opencv.hpp"
#include "math/types.h"
// Forward declaration
namespace cv {
class VideoCapture;
}

class Camera  : public QObject
{
    Q_OBJECT
public:
    enum class CameraState
    {
        DISABLED = 0,
        CONNECTING = 1,
        RUNNING = 2,
    };

    Camera();
    void Activate(bool activate);
    void Update();
    CameraState GetState() const {return _state; }
    static cv::Size const & GetResolution() {return _resolution;}
    static cv::Point2f const & GetMidPoint() {return _midPoint;}


    static cv::Point2f ConvertCameraToMidPointCoord(cv::Point2f cameraPt);
    static cv::Point2f ConvertMidPointToCameraCoord(cv::Point2f midPt);

    static Point3f ConvertMidPointToCrazyFlieCoordinates(Point3f positionEstimateCamera);

    static std::vector<Blob> ConvertCameraToCrazyFlieCoord(std::vector<cv::KeyPoint> const & keyPoints);
    static Blob ConvertCameraToCrazyFlieCoord(cv::KeyPoint cameraPt);
    static Point3f ConvertCrazyFlieToMidPointCoord(Blob blob);
    static cv::Point2f ConvertCrazyFlieToCameraCoord(Blob blob);
signals:
    void ImgReadyForDisplay(QImage const &);
    void ImgReadyForProcessing(cv::Mat const &);
    void ImgReadyForInitialization(cv::Mat const &);

private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    static cv::Size _resolution;
    static cv::Point2f _midPoint;

    void FetchAndImageReady();
    void FetchImage(cv::Mat & frame);
    void InitializeTracking();
    static float EstimateDistance(const cv::KeyPoint & point);
};
