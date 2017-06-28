#pragma once
#include <QWidget>




class CameraViewPainter : public QWidget
{
//    Q_OBJECT
public:
    void paintEvent(QPaintEvent* /*event*/);
public slots:
private slots:
private:
    static constexpr qreal _yaw_max = 60; // 2*yaw_max is the total x coord
    static constexpr qreal _pitch_max = 30; // 2*pitch_max is the total y coord

    QPoint World2CameraCoord(QPointF point);
    void PaintHorizontalLine(float factor, float distance, QPainter & painter);
    void PaintVerticalLine(int factor, QPainter & painter);

};




