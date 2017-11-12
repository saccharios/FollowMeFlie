#pragma once
#include <QObject>
#include <QColor>
#include "opencv2/opencv.hpp"

struct Distance
{
   float x;
   float y;
   float z;
};


class ExtractColor :  public QObject
{
    Q_OBJECT

public:
    ExtractColor(QColor const & color) : _colorToFilter(color) {}

public slots:
    void ProcessImage(cv::Mat const & img);

signals:
    void NewDistance(Distance const &);

private:
    QColor const & _colorToFilter;

    void ConvertToHSV(cv::Mat const & img, cv::Mat & imgHSV, cv::Scalar & colorLower, cv::Scalar colorUpper);
    void FilterImage(cv::Mat & imgThresholded);
    cv::SimpleBlobDetector::Params CreateParameters();
    void DrawBlobs(cv::Mat const & img, std::vector<cv::KeyPoint> const & keyPoints);
    Distance CalculateDistance(cv::KeyPoint const & largestKeyPoint,
                               cv::Size cameraSize,
                               double blob_size_to_length,
                               double focal_length,
                               double size_ball,
                               double field_of_view);
};
