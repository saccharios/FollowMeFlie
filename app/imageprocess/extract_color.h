#pragma once
#include <QObject>
#include <QColor>
#include "opencv2/opencv.hpp"
#include "ball_kalman_filter_2d.h"
#include "ball_kalman_filter_3d.h"
#include "math/types.h"
#include "time_levels.h"
#include "camera.h"
class ExtractColor :  public QObject
{
    Q_OBJECT

public:


    ExtractColor(QColor const & color) :
        _colorToFilter(color),
        _kalmanFilter_2d(1, 0.05, 0.1), // meas_noise, process_noise_1, process_noise_2
        _kalmanFilter_3d(0.5, 0.05, 0.0), // meas_noise, process_noise_1, process_noise_2
        _detectorParams()
    {

        // Change thresholds
        _detectorParams.minThreshold = 10;
        _detectorParams.maxThreshold = 255;
        // Filter by Area.
        _detectorParams.filterByArea = true;
        _detectorParams.minArea = 3;
        _detectorParams.maxArea = 100000;

        // Filter by Circularity
        _detectorParams.filterByCircularity = true;
        _detectorParams.minCircularity = 0.4;

        // Filter by Convexity
        _detectorParams.filterByConvexity = true;
        _detectorParams.minConvexity = 0.6;
        _detectorParams.maxConvexity = 1.0;

        // Filter by Inertia
        _detectorParams.filterByInertia = true;
        _detectorParams.minInertiaRatio = 0.2;
        _detectorParams.maxInertiaRatio = 1.0;

        _detectorParams.filterByColor = true;
        _detectorParams.blobColor = 0;
    }

public slots:
    void ProcessImage(cv::Mat const & img);
    void Initialize(cv::Mat const & img);
    void StartMeasurement() {_kalmanFilter_2d.StartMeasurement(true);}
    void SetSetPoint(Point3f setPoint) {_setPoint = setPoint;}

signals:
    void BallEstimateReady(Point3f const &);

private:
    QColor const & _colorToFilter;
    BallKalmanFilter_2d _kalmanFilter_2d;
    BallKalmanFilter_3d _kalmanFilter_3d;

    cv::SimpleBlobDetector::Params _detectorParams;
    int fiftyCmInRadius = 18;
    static constexpr float _fiftyCmInRadius = 18;
    static constexpr float _factor = _fiftyCmInRadius / 33.88;

    Point3f _setPoint = {0.5f,0.0f,0.0f};

    void ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper);
    std::vector<cv::KeyPoint> ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgToShow);
    void FilterImage(cv::Mat & imgThresholded);

    void RemoveKeyPointsAtEdges(std::vector<cv::KeyPoint> & blobs);
    Point3f ProcessWithKalman2d(std::vector<cv::KeyPoint> const & camPoints);
    Point3f ProcessWithKalman3d(std::vector<cv::KeyPoint> const & camPoints);
    void DrawEstimate(cv::Mat & imgToShow, Point3f estimateCrazyFlieCoord, cv::Scalar color);
};
