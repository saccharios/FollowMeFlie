#pragma once
#include <QWidget>




class CameraViewPainter : public QWidget
{
//    Q_OBJECT
public:
    CameraViewPainter(float const & roll) : _roll(roll) {}
    void paintEvent(QPaintEvent* /*event*/);
public slots:
private slots:
private:
    static constexpr qreal _x_max = 60; // 2*yaw_max is the total x coord
    static constexpr qreal _y_max = 30; // 2*pitch_max is the total y coord

    float const & _roll;

    QPoint World2CameraCoord(QPointF point);
    void PaintHorizontalLine(float factor, float distance, QPainter & painter);
    void PaintVerticalLine(int factor, QPainter & painter);
    void DrawGround(QPainter & painter, float angle);

};




