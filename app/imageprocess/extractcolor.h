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
    ExtractColor(QColor const & color) : _colorToFilter(color)
    {}


public slots:
    void ProcessImage(cv::Mat const & img);

signals:
    void NewDistance(Distance const &);

private:
    QColor const &
    _colorToFilter;

};
