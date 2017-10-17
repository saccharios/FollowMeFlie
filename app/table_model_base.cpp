#include "table_model_base.h"
#include <iostream>
TableModelBase::TableModelBase(std::vector<TOCElement> const & tocElements, QObject *parent)
    : QAbstractTableModel(parent),
      _tocElements(tocElements)
{
    _header[Columns::Index] = QString("Index");
    _header[Columns::Group] = QString("Group");
    _header[Columns::Name] = QString("Name");
    _header[Columns::Value] = QString("Value");
}

QVariant TableModelBase::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            return _header.at(static_cast<Columns>(section));
        }
    }
    return QVariant();
}



int TableModelBase::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return _tocElements.size();
}

bool TableModelBase::insertRows(int row, int count, const QModelIndex &parent )
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), row, row + count - 1);
//    for (int i = 0; i < count; ++i)
//    {
//        TOCElement e;
//        _tocElements.push_back(e);
//    }
    endInsertRows();
    return true;
}

bool TableModelBase::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), row, row + count - 1);

//    for (int i = 0; i < count; ++i) {
//        _tocElements.pop_back();
//    }
    endRemoveRows();
    return true;

}


