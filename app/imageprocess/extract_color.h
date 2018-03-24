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

    struct Blobs
    {
        std::vector<cv::KeyPoint> camPoints;
        std::vector<cv::KeyPoint> midPoints;
    };

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
        _detectorParams.filterByCircularity = false;
        _detectorParams.minCircularity = 0.1;

        // Filter by Convexity
        _detectorParams.filterByConvexity = false;
        _detectorParams.minConvexity = 0.87;

        // Filter by Inertia
        _detectorParams.filterByInertia = false;
        _detectorParams.minInertiaRatio = 0.01;
    }

public slots:
    void ProcessImage(cv::Mat const & img);
    void Initialize(cv::Mat const & img);

signals:
    void EstimateReady(Distance const &);

private:
    QColor const & _colorToFilter;
    BallKalmanFilter _kalmanFilter;

    cv::SimpleBlobDetector::Params _detectorParams;


    void ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper);
    ExtractColor::Blobs ExtractKeyPoints(cv::Mat const & img, cv::Mat & imgToShow);
    void FilterImage(cv::Mat & imgThresholded);
    float CalculateDistance(const cv::KeyPoint &point);
    void RemoveKeyPointsAtEdges(Blobs & blobs);
    void AddMidPtCoord(Blobs & blobs);
};
