#include "qt_util.h"
#include <QPoint>
#include <QPointF>
#include <QTableView>
#include <QScrollBar>
#include <QHeaderView>

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


void SetupTableViewWidget(QTableView* tableView)
{
    // Hide vertical header
    tableView->verticalHeader()->hide();
    // Resize columns and rows to fit content
//    tableView->resizeColumnsToContents();
//    tableView->resizeRowsToContents();
    // Resize TableView Widget to match content size
    int w = 0;
    int h = 0;
    w += tableView->contentsMargins().left() + tableView->contentsMargins().right();
    w += tableView->horizontalScrollBar()->width()/4; // TODO SF: Somehow the width of the horizontalScrollBar is way too large?
    h +=  tableView->contentsMargins().top()+ tableView->contentsMargins().bottom();
    h +=  tableView->horizontalHeader()->height();
    for (int i=0; i<tableView->model()->columnCount(); ++i)
    {
        w += tableView->columnWidth(i);
    }
    for (int i=0; i < 20; ++i) // Minimum 6 rows are shown.
    {
        h += tableView->rowHeight(i);
    }

    tableView->setMinimumWidth(w);
    tableView->setMaximumWidth(w);
    tableView->setMinimumHeight(h);
//    tableView->setMaximumHeight(h);


    tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}





}
