#include "camera_view_painter.h"
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
#include <QColor>
#include <QRect>

CameraViewPainter::CameraViewPainter(float const & roll, float const & yaw, float const & pitch) :
    _roll(roll),
    _yaw(yaw),
    _pitch(pitch),
    _backGround(&_horizon),
    _horizon(this),
    _cameraBackGround(this)
{
    // set blue background
    QPalette pal;
    QColor blue(25,25,112);
    pal.setColor(QPalette::Background, blue);
    setAutoFillBackground(true);
    setPalette(pal);
    show();
}


QPoint World2CameraCoord(QPointF point_world, QWidget* widget, float x_max, float y_max )
{
    //Converts world coordinates to camera coordinates for plotting
    QSize widgetSize = widget->size();
    QPoint point_camera;
    point_camera.setX((point_world.x() / x_max + 1) * widgetSize.width() / 2);
    point_camera.setY((-point_world.y() / y_max + 1) * widgetSize.height() / 2);
    return point_camera;
}

void CameraViewPainter::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    _backGround->DrawGround(painter, _roll, _pitch, _x_max, _y_max);
    // Setup static middle line
    painter.setPen(Qt::black);
    QPoint p1 = World2CameraCoord(QPointF{-_x_max, 0.0}, this, _x_max, _y_max);
    QPoint p2 = World2CameraCoord(QPointF{_x_max, 0.0}, this, _x_max, _y_max);
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
    // draw above
    float y = WrapAround(pitch, 0.0f, distance_between_lines);
    y = distance_between_lines - y;
    while(y < _y_max)
    {
        QPoint p1 = World2CameraCoord(qt_utils::rotate(QPointF{-length, y}, -roll_r),this, _x_max, _y_max);
        QPoint p2 = World2CameraCoord(qt_utils::rotate(QPointF{length, y}, -roll_r),this, _x_max, _y_max);
        painter.drawLine(QLine(p1,p2));
        y += distance_between_lines;
    }
    // draw below
    y = WrapAround(pitch, 0.0f, distance_between_lines);
    y =  - y;
    while(y > -_y_max)
    {
        QPoint p1 = World2CameraCoord(qt_utils::rotate(QPointF{-length, y}, -roll_r),this, _x_max, _y_max);
        QPoint p2 = World2CameraCoord(qt_utils::rotate(QPointF{length, y}, -roll_r),this, _x_max, _y_max);
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
    while(x < _x_max)
    {
        QPoint p1 = World2CameraCoord(qt_utils::rotate(QPointF{x,-length}, -roll_r),this, _x_max, _y_max);
        QPoint p2 = World2CameraCoord(qt_utils::rotate(QPointF{x, length}, -roll_r),this, _x_max, _y_max);
        painter.drawLine(QLine(p1,p2));
        QPoint p3 = World2CameraCoord(qt_utils::rotate(QPointF{x, 0} + text_offset,-roll_r),this, _x_max, _y_max);
        float num = WrapAround(yaw-x,-180.0f, 180.0f);
        // Wrapping may not return exactly zero:
        if( std::abs(num) < 0.00005)
        {
            num = 0.0;
        }
        painter.drawText(p3,QString::number(num));
        x += distance_between_lines;
    }
    // draw to the left
    x = WrapAround(yaw, 0.0f, distance_between_lines);
    x = x - distance_between_lines;
    while(x > -_x_max)
    {
        QPoint p1 = World2CameraCoord(qt_utils::rotate(QPointF{x, -length}, -roll_r),this, _x_max, _y_max);
        QPoint p2 = World2CameraCoord(qt_utils::rotate(QPointF{x, length}, -roll_r),this, _x_max, _y_max);
        painter.drawLine(QLine(p1,p2));
        QPoint p3 = World2CameraCoord(qt_utils::rotate(QPointF{x, 0} + text_offset,-roll_r),this, _x_max, _y_max);
        float num = WrapAround(yaw-x,-180.0f, 180.0f);
        // Wrapping may not return exactly zero:
        if( std::abs(num) < 0.00005)
        {
            num = 0.0;
        }
        painter.drawText(p3,QString::number(num));
        x -= distance_between_lines;
    }
    // Line in the middle
    QPoint p1 = World2CameraCoord(qt_utils::rotate(QPointF{0.0,-2.0*length}, -roll_r),this, _x_max, _y_max);
    QPoint p2 = World2CameraCoord(qt_utils::rotate(QPointF{0.0, 2.0*length}, -roll_r),this, _x_max, _y_max);
    painter.drawLine(QLine(p1,p2));
}



void Horizon::DrawGround(QPainter & painter, float roll, float pitch,float x_max, float y_max) // roll, pitch in degree
{
    auto roll_r = deg2rad(roll);
    QVector<QPoint> points;
    // Magic number 10, so that the ground is always there
    QPoint p1 = World2CameraCoord(qt_utils::rotate(QPointF{-10.0*x_max, -pitch}, -roll_r), _widget, x_max, y_max);
    QPoint p2 = World2CameraCoord(qt_utils::rotate(QPointF{-10.0*x_max, -4.0*y_max-pitch}, -roll_r), _widget, x_max, y_max);
    QPoint p3 = World2CameraCoord(qt_utils::rotate(QPointF{10.0*x_max, -4.0*y_max-pitch}, -roll_r), _widget, x_max, y_max);
    QPoint p4 = World2CameraCoord(qt_utils::rotate(QPointF{10.0*x_max, -pitch}, -roll_r), _widget, x_max, y_max);
    points.append(p1);
    points.append(p2);
    points.append(p3);
    points.append(p4);
    QColor green(0,100,0);
    painter.setPen(green);
    painter.setBrush(green);
    QPolygon ground(points);
    painter.drawPolygon(ground);
}
void CameraBackGround::DrawGround(QPainter & painter, float /*roll*/, float /*pitch*/,float /*x_max*/, float /*y_max*/) // roll, pitch in degree
{
    QSize widgetSize = _widget->size();
    QRect rect(0,0,widgetSize.width(),widgetSize.height());
    painter.drawImage(rect, _image);


}
