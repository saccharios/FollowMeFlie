#pragma once
#include <QPoint>
#include <QPointF>
namespace qt_utils
{

QPoint rotate(QPoint const & p , float const & angle) // angle in radians
{
    float cosa = cos(angle);
    float sina = sin(angle);
    QPoint q;
    q.setX(p.x()*cosa - p.y() * sina);
    q.setY(p.x()*sina + p.y() * cosa);
    return q;
}

QPointF rotate(QPointF const & p , float const & angle) // angle in radians
{
    float cosa = cos(angle);
    float sina = sin(angle);
    QPointF q;
    q.setX(p.x()*cosa - p.y() * sina);
    q.setY(p.x()*sina + p.y() * cosa);
    return q;
}

}
