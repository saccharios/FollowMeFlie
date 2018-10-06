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
    bool IsDisabled() const;
    bool IsActive() const;
    static cv::Size const & GetResolution() {return _resolution;}
    static cv::KeyPoint const & GetOrigin() {return _origin;}


    static MidPoint ConvertCameraToMidPointCoord(cv::KeyPoint keyPoint);
    static std::vector<MidPoint> ConvertCameraToMidPointCoord(std::vector<cv::KeyPoint> const & keyPoints);

    static cv::KeyPoint ConvertMidPointToCameraCoord(MidPoint midPoint);
    static std::vector<cv::KeyPoint> ConvertMidPointToCameraCoord(std::vector<MidPoint> const & midPoints);

    static Point3f ConvertMidPointToCrazyFlieCoord(MidPoint midPoint);

    static MidPoint ConvertCrazyFlieCoordToMidPoint(Point3f crazyFliePoint);

    static Point3f ConvertCameraToCrazyFlieCoord(cv::KeyPoint keyPoint);
    static std::vector<Point3f> ConvertCameraToCrazyFlieCoord(std::vector<cv::KeyPoint> const & keyPoints);

signals:
    void ImgReadyForDisplay(QImage const &);
    void ImgReadyForProcessing(cv::Mat const &);
    void ImgReadyForInitialization(cv::Mat const &);

    void CameraIsRunning(bool const &);

private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    static cv::Size _resolution;
    static cv::KeyPoint _origin;

    void FetchAndImageReady();
    void FetchImage(cv::Mat & frame);
    void InitializeTracking();
    void SetResolution(cv::Size resolution);
    static constexpr float _focalLength = 223.5;
    static constexpr float _a = 1670.0f;
    static constexpr float _b =  -0.711f;
};
