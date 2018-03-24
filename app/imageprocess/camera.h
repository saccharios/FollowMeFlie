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

    Camera();
    void Activate(bool activate);
    void Update();
    CameraState GetState() const {return _state; }
    static cv::Size const & GetResolution() {return _resolution;}
    static cv::Point2f const & GetMidPoint() {return _midPoint;}



    static cv::Point2f ConvertCameraToMidPointCoord(cv::Point2f cameraPt);
    static cv::Point2f ConvertMidPointToCameraCoord(cv::Point2f midPt);
    static std::vector<cv::KeyPoint> ConvertCameraToMidPointCoord(std::vector<cv::KeyPoint> const & keyPoints);

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
};
