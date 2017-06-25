#include "cameraviewpainter.h"
#include "qpainter.h"
#include <iostream>

void CameraViewPainter::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, "Qt");
}
