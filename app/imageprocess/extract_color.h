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
        _kalmanFilter(0.5, 0.05, 0.0),
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
    void StartMeasurement() {_kalmanFilter.StartMeasurement(true);}

signals:
    void EstimateReady(Point3f const &);

private:
    QColor const & _colorToFilter;
    BallKalmanFilter _kalmanFilter;

    cv::SimpleBlobDetector::Params _detectorParams;


    void ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper);
    std::vector<cv::KeyPoint> ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgToShow);
    void FilterImage(cv::Mat & imgThresholded);

    void RemoveKeyPointsAtEdges(std::vector<cv::KeyPoint> & blobs);
};
