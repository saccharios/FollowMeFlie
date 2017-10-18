#pragma once
#include <QPoint>
#include <QPointF>
#include <QTableView>

namespace qt_utils
{

QPoint rotate(QPoint const & p , float const & angle) ;// angle in radians
QPointF rotate(QPointF const & p , float const & angle); // angle in radians
void SetupTableViewWidget(QTableView* tableView);

}
