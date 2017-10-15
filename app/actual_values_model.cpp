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

    return rows;
//    if (parent.isValid())
//        return 0;

    // FIXME: Implement me!
}

int ActualValuesModel::columnCount(const QModelIndex &parent) const
{

        return static_cast<int>(Columns::Count);
    //    if (parent.isValid())
//        return 0;

    // FIXME: Implement me!
}
QVariant ActualValuesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

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
            //save value from editor to member m_gridData
            _values[index.row()] = value.toString();
            //for presentation purposes only: build and emit a joined string
//            QString result;
//            for(int row= 0; row < 2; row++)
//            {
//                for(int col= 0; col < 4; col++)
//                {
//                    result += m_gridData[row][col] + " ";
//                }
//            }
            emit editCompleted( _values[index.row()] );
        }
    }
    return true;

//    if (data(index, role) != value) {
//        // FIXME: Implement me!
//        emit dataChanged(index, index, QVector<int>() << role);
//        return true;
//    }
//    return false;
}

Qt::ItemFlags ActualValuesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

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
