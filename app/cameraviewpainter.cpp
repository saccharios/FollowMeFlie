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

CameraViewPainter::CameraViewPainter(float const & roll, float const & yaw, float const & pitch) : _roll(roll), _yaw(yaw), _pitch(pitch)
{
    // set blue background
    QPalette pal;
    pal.setColor(QPalette::Background, Qt::blue);
    setAutoFillBackground(true);
    setPalette(pal);
    show();
}


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

    DrawGround(painter, _roll, _pitch);
    // Setup static middle line
    painter.setPen(Qt::black);
    QPoint p1 = World2CameraCoord(QPointF{-_x_max, 0.0});
    QPoint p2 = World2CameraCoord(QPointF{_x_max, 0.0});
//        QPoint p2 = World2CameraCoord({ length,y});
    painter.drawLine(QLine(p1,p2));
    // Setup horizontal lines
    PaintHorizontalLine(painter, _roll, _pitch);
    // Setup vertical lines
    PaintVerticalLine(painter, _roll,  _yaw);
}


void CameraViewPainter::PaintHorizontalLine(QPainter & painter, float roll, float pitch)// angles in degree
{
    auto roll_r = deg2rad(roll);
    painter.setPen(Qt::white);
    float length = _x_max * 0.75;
    float distance_between_lines = 10;
//    QPointF text_offset{0,2};
    // draw above
    float y = WrapAround(pitch, 0.0f, distance_between_lines);
    y = distance_between_lines - y;
    while(y < _y_max)
    {
        QPoint p1 = World2CameraCoord(rotate(QPointF{-length, y}, -roll_r));
        QPoint p2 = World2CameraCoord(rotate(QPointF{length, y}, -roll_r));
        painter.drawLine(QLine(p1,p2));
        y += distance_between_lines;
    }
    // draw below
    y = WrapAround(pitch, 0.0f, distance_between_lines);
    y =  - y;
    while(y > -_y_max)
    {
        QPoint p1 = World2CameraCoord(rotate(QPointF{-length, y}, -roll_r));
        QPoint p2 = World2CameraCoord(rotate(QPointF{length, y}, -roll_r));
        painter.drawLine(QLine(p1,p2));
        y -= distance_between_lines;
    }
}
void CameraViewPainter::PaintVerticalLine(QPainter & painter, float roll, float yaw) // angles in degree
{
    auto roll_r = deg2rad(roll);
    painter.setPen(Qt::white);
    float length= 1;
    float distance_between_lines = 10;
    QPointF text_offset{0,2};
    // draw to the right
    float x = WrapAround(yaw, 0.0f, distance_between_lines);
    x = distance_between_lines - x;
    while(x < _x_max)
    {
        QPoint p1 = World2CameraCoord(rotate(QPointF{x,-length}, -roll_r));
        QPoint p2 = World2CameraCoord(rotate(QPointF{x, length}, -roll_r));
        painter.drawLine(QLine(p1,p2));
        QPoint p3 = World2CameraCoord(rotate(QPointF{x, 0} + text_offset,-roll_r));
        float num = WrapAround(x+yaw,-180.0f, 180.0f);
        // Wrapping may not return exactly zero:
        if( std::abs(num) < 0.0005)
        {
            num = 0.0;
        }
        painter.drawText(p3,QString::number(num));
        x += distance_between_lines;
    }
    // draw to the left
    x = WrapAround(yaw, 0.0f, distance_between_lines);
    x =  - x;
    while(x > -_x_max)
    {
        QPoint p1 = World2CameraCoord(rotate(QPointF{x, -length}, -roll_r));
        QPoint p2 = World2CameraCoord(rotate(QPointF{x, length}, -roll_r));
        painter.drawLine(QLine(p1,p2));
        QPoint p3 = World2CameraCoord(rotate(QPointF{x, 0} + text_offset,-roll_r));
        painter.drawText(p3,QString::number(WrapAround(x+yaw,-180.0f, 180.0f)));
        x -= distance_between_lines;
    }
    // Line in the middle
    QPoint p1 = World2CameraCoord(rotate(QPointF{0.0,-2.0*length}, -roll_r));
    QPoint p2 = World2CameraCoord(rotate(QPointF{0.0, 2.0*length}, -roll_r));
    painter.drawLine(QLine(p1,p2));
}



void CameraViewPainter::DrawGround(QPainter & painter, float roll, float pitch) // roll, pitch in degree
{
    auto roll_r = deg2rad(roll);
    QVector<QPoint> points;
// TODO SF What is the maximum? instead of 10?
    QPoint p1 = World2CameraCoord(rotate(QPointF{-10.0*_x_max, -pitch}, -roll_r));
    QPoint p2 = World2CameraCoord(rotate(QPointF{-10.0*_x_max, -4.0*_y_max-pitch}, -roll_r));
    QPoint p3 = World2CameraCoord(rotate(QPointF{10.0*_x_max, -4.0*_y_max-pitch}, -roll_r));
    QPoint p4 = World2CameraCoord(rotate(QPointF{10.0*_x_max, -pitch}, -roll_r));
    points.append(p1);
    points.append(p2);
    points.append(p3);
    points.append(p4);
    painter.setPen(Qt::green);
    painter.setBrush(Qt::green);
    QPolygon ground(points);
    painter.drawPolygon(ground);

}
