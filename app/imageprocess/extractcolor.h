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
    ExtractColor() : _colorToFilter()
    {}


public slots:
    void ProcessImage(cv::Mat const & img);
    void SetColor(QColor const & color) {_colorToFilter = color;}

signals:
    void NewDistance(Distance const &);

private:
    QColor
    _colorToFilter;

};
