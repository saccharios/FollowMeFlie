#include "cameraviewpainter.h"
#include <QPainter>
#include <QPoint>
#include <QPointF>
#include <QtCore/qmath.h>
#include "math/functions.h"
#include <math/constants.h>
#include <QPolygon>
#include <QVector>
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
    // TODO For testing only
    static float angle = 0.0;
    angle += 0.025;
    angle = WrapAround(angle, -pi, pi);
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

void CameraViewPainter::DrawGround(QPainter & painter, float angle)
{
    std::cout << "curernt angle = " << angle << std::endl;
    QVector<QPoint> points;
    float edge_angle = qAtan2(size().height(), size().width());
    float tan_angle = tan(angle);
    if( - edge_angle < angle && angle <= edge_angle )
    {
        QPoint p1 = World2CameraCoord({-_x_max, -_x_max * tan_angle});
        QPoint p2 = World2CameraCoord({-_x_max, -_y_max});
        QPoint p3 = World2CameraCoord({_x_max, -_y_max});
        QPoint p4 = World2CameraCoord({_x_max, _x_max * tan_angle});
        points.append(p1);
        points.append(p2);
        points.append(p3);
        points.append(p4);
        std::cout << "case 1" << std::endl;
    }
    else if (edge_angle < angle && angle <= pi - edge_angle)
    {
        QPoint p1 = World2CameraCoord({-_y_max / tan_angle, -_y_max });
        QPoint p2 = World2CameraCoord({_x_max, -_y_max});
        QPoint p3 = World2CameraCoord({_x_max, _y_max});
        QPoint p4 = World2CameraCoord({_y_max / tan_angle, _y_max});
        points.append(p1);
        points.append(p2);
        points.append(p3);
        points.append(p4);
        std::cout << "case 2"<< std::endl;
    }
    else if (-pi + edge_angle > angle || angle > pi -edge_angle)
    {
        QPoint p1 = World2CameraCoord({_x_max , _x_max * tan_angle });
        QPoint p2 = World2CameraCoord({_x_max, _y_max});
        QPoint p3 = World2CameraCoord({-_x_max, _y_max});
        QPoint p4 = World2CameraCoord({-_x_max , -_x_max * tan_angle});
        points.append(p1);
        points.append(p2);
        points.append(p3);
        points.append(p4);
        std::cout << "case 3"<< std::endl;

    }
    else
    {
        QPoint p1 = World2CameraCoord({_y_max / tan_angle, _y_max });
        QPoint p2 = World2CameraCoord({-_x_max, _y_max});
        QPoint p3 = World2CameraCoord({-_x_max, -_y_max});
        QPoint p4 = World2CameraCoord({-_y_max / tan_angle, -_y_max});
        points.append(p1);
        points.append(p2);
        points.append(p3);
        points.append(p4);
        std::cout << "case 4"<< std::endl;
    }

    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    QPolygon ground(points);
    painter.drawPolygon(ground);




}
