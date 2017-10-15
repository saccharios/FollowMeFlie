#include "actual_values_model.h"

ActualValuesModel::ActualValuesModel(QObject *parent):
    QAbstractTableModel(parent)
{
    _header[Columns::Index] = QString("Index");
    _header[Columns::Group] = QString("Group");
    _header[Columns::Name] = QString("Name");
    _header[Columns::Value] = QString("Value");
    _header[Columns::Edit] = QString("Edit");


}

QVariant ActualValuesModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int ActualValuesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return rows;
}

int ActualValuesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return cols;
}
QVariant ActualValuesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if(index.column() == static_cast<int>(Columns::Edit))
        {
            return _values[index.row()];
        }
        else
        {
            return QString("Row%1, Column%2")
                    .arg(index.row() + 1)
                    .arg(index.column() +1);
        }
    }
    return QVariant();
}

bool ActualValuesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if(index.column() == static_cast<int>(Columns::Edit))
    {
        if (role == Qt::EditRole)
        {
            _values[index.row()] = value.toString();
            emit editCompleted( _values[index.row()] );
        }
    }
    return true;

}

Qt::ItemFlags ActualValuesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if(index.column() == static_cast<int>(Columns::Edit))
    {
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    }
    return Qt::NoItemFlags;
}

//bool ActualValuesModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    beginInsertRows(parent, row, row + count - 1);
//    // FIXME: Implement me!
//    endInsertRows();
//}

//bool ActualValuesModel::insertColumns(int column, int count, const QModelIndex &parent)
//{
//    beginInsertColumns(parent, column, column + count - 1);
//    // FIXME: Implement me!
//    endInsertColumns();
//}

//bool ActualValuesModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    beginRemoveRows(parent, row, row + count - 1);
//    // FIXME: Implement me!
//    endRemoveRows();
//}

//bool ActualValuesModel::removeColumns(int column, int count, const QModelIndex &parent)
//{
//    beginRemoveColumns(parent, column, column + count - 1);
//    // FIXME: Implement me!
//    endRemoveColumns();
//}
