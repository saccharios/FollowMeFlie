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
    static cv::KeyPoint const & GetOrigin() {return _origin;}


    static MidPoint ConvertCameraToMidPointCoord(cv::KeyPoint cameraPt);
    static std::vector<MidPoint> ConvertCameraToMidPointCoord(std::vector<cv::KeyPoint> const & keyPoints);
    static cv::KeyPoint ConvertMidPointToCameraCoord(MidPoint midPt);
    static std::vector<cv::KeyPoint> ConvertMidPointToCameraCoord(std::vector<MidPoint> const & midPoints);

    static Point3f ConvertMidPointToCrazyFlieCoord(MidPoint midPoint);
    static MidPoint ConvertCrazyFlieCoordToMidPoint(Point3f crazyFliePoint);

signals:
    void ImgReadyForDisplay(QImage const &);
    void ImgReadyForProcessing(cv::Mat const &);
    void ImgReadyForInitialization(cv::Mat const &);

private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    static cv::Size _resolution;
    static cv::KeyPoint _origin;

    void FetchAndImageReady();
    void FetchImage(cv::Mat & frame);
    void InitializeTracking();
    static constexpr float _focalLength = 223.5;
};
