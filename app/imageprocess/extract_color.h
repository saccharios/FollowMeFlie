#pragma once
#include <QObject>
#include <QColor>
#include "opencv2/opencv.hpp"
#include "ball_kalman_filter.h"
#include "math/types.h"
#include "time_levels.h"
#include "camera.h"
class ExtractColor :  public QObject
{
    Q_OBJECT

public:
    ExtractColor(QColor const & color) :
        _colorToFilter(color),
      _kalmanFilter(1.0, 1.0, 0.0)
    {}

public slots:
    void ProcessImage(cv::Mat const & img);
    void Initialize(cv::Mat const & img);

signals:
    void NewDistance(Distance const &);

private:
    QColor const & _colorToFilter;
    BallKalmanFilter _kalmanFilter;

    void ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper);
    std::vector<cv::KeyPoint> ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgWithKeypoints);
    void FilterImage(cv::Mat & imgThresholded);
    cv::SimpleBlobDetector::Params CreateParameters();
    Distance CalculateDistance(cv::Point2f point,
                               double size,
                               cv::Size cameraSize,
                               double blobSizeToLength,
                               double focalLength,
                               double sizeBall,
                               double fieldOfView);
};
