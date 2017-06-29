#include "cameraviewpainter.h"
#include <QPainter>
#include <QPoint>
#include <QPointF>
#include <QtCore/qmath.h>
#include "math/functions.h"
#include <math/constants.h>
#include <QPolygon>
#include <QVector>
#include "qt_util.h"
#include <iostream>
QPoint CameraViewPainter::World2CameraCoord(QPointF point_world)
{
    QSize widgetSize = size();
    QPoint point_camera;
    point_camera.setX((point_world.x() / _x_max + 1) * widgetSize.width() / 2);
    point_camera.setY((-point_world.y() / _y_max + 1) * widgetSize.height() / 2);
    return point_camera;
}

void CameraViewPainter::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setPen(Qt::white);
    // Setup horizontal lines
    std::array<float,3> scaling = {1.0, 0.8, 0.6};
    std::array<int,3> distance = {0, 10, 20};
    PaintHorizontalLine(scaling[0], distance[0], painter);
    PaintHorizontalLine(scaling[1], distance[1], painter);
    PaintHorizontalLine(scaling[1], -distance[1], painter);
    PaintHorizontalLine(scaling[2], distance[2], painter);
    PaintHorizontalLine(scaling[2], -distance[2], painter);
    // Setup vertical lines
    for(int i = -3; i<4;++i)
    {
        PaintVerticalLine(i, painter);
    }
    float angle = WrapAround(_roll/180.0f*pi, -pi, pi);
    DrawGround(painter, angle);
}


void CameraViewPainter::PaintHorizontalLine(float factor, float distance, QPainter & painter)
{
    float x_scaling = 0.9;
    auto point1 = World2CameraCoord({-_x_max*x_scaling*factor,distance});
    auto point2 = World2CameraCoord({_x_max*x_scaling*factor,distance});
    painter.drawLine(QLine(point1,point2));
}
void CameraViewPainter::PaintVerticalLine(int factor, QPainter & painter)
{
    float length= 1;
    float distance = 15;
    auto point1 = World2CameraCoord({distance*factor,length});
    auto point2 = World2CameraCoord({distance*factor,-length});
    painter.drawLine(QLine(point1,point2));
}



void CameraViewPainter::DrawGround(QPainter & painter, float angle) // angle is in radians
{
    QVector<QPoint> points;
    QPoint p1 = World2CameraCoord(rotate(QPointF{-4.0*_x_max, 0}, -angle));
    QPoint p2 = World2CameraCoord(rotate(QPointF{-4.0*_x_max, -4.0*_y_max}, -angle));
    QPoint p3 = World2CameraCoord(rotate(QPointF{4.0*_x_max, -4.0*_y_max}, -angle));
    QPoint p4 = World2CameraCoord(rotate(QPointF{4.0*_x_max, 0}, -angle));
    points.append(p1);
    points.append(p2);
    points.append(p3);
    points.append(p4);
    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    QPolygon ground(points);
    painter.drawPolygon(ground);

}
