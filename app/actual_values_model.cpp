#include "actual_values_model.h"

ActualValuesModel::ActualValuesModel(std::vector<TOCElement> const & tocElements, QObject *parent):
    Base(tocElements, parent)
{
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

int ActualValuesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return cols;
}


QVariant ActualValuesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();


    if (role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            return QString::number(_tocElements.at(index.row()).id);
        }
        if(index.column() == 1)
        {
            return QString::fromStdString(_tocElements.at(index.row()).group);
        }
        if(index.column() == 2)
        {
            return QString::fromStdString(_tocElements.at(index.row()).name_only);
        }
        if(index.column() == 3)
        {
            return QString::number(_tocElements.at(index.row()).value);
        }
    }
    return QVariant();
}

void ActualValuesModel::UpdateActualValues()
{
    QModelIndex topLeft = createIndex(0,static_cast<int>(Columns::Value));
    QModelIndex bottomRight = createIndex(_tocElements.size(),static_cast<int>(Columns::Value));
    emit dataChanged(topLeft, bottomRight);
}

