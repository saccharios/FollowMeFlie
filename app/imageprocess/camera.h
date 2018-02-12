#pragma once
#include <QObject>
#include<memory>
#include "opencv2/opencv.hpp"

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
    struct CameraResolution
    {
        int width = 0;
        int height = 0;
    };

    Camera();
    void Activate(bool activate);
    void Update();
    CameraState GetState() const {return _state; }
    CameraResolution const & GetResolution() const {return _resolution;}



    static cv::Point2f  ConvertCameraToMidPointCoord(cv::Point2f cameraPt, cv::Size size);
    static cv::Point2f  ConvertMidPointToCameraCoord(cv::Point2f midPt, cv::Size size);

signals:
    void ImgReadyForDisplay(QImage const &);
    void ImgReadyForProcessing(cv::Mat const &);
private:
    CameraState _state;
    bool _activated;

    cv::VideoCapture* _capture;

    CameraResolution _resolution;
    void FetchImage();
};
