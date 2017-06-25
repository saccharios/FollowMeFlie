#include "cameraviewpainter.h"
#include <QPainter>
#include <QPoint>
#include <iostream>

void CameraViewPainter::paintEvent(QPaintEvent* /*event*/)
{
    QSize widgetSize = size();


    QPoint point1(0,widgetSize.height() / 2);
    QPoint point2(widgetSize.width() ,widgetSize.height() / 2);


    QPainter painter(this);
    painter.setPen(Qt::white);
    painter.drawLine(point1, point2);

}
