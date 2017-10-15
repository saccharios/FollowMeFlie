#include "table_model_base.h"

TableModelBase::TableModelBase(QObject *parent)
    : QAbstractItemModel(parent)
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

int TableModelBase::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return 0;
    }

        return rows;
}

int TableModelBase::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return 0;
    }

    return cols;
}

QVariant TableModelBase::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
       return QString("Row%1, Column%2")
                   .arg(index.row() + 1)
                   .arg(index.column() +1);
    }
    return QVariant();
}
