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
        if(index.column() == static_cast<int>(Columns::Index))
        {
            return QString::number(_tocElements.at(index.row()).id);
        }
        if(index.column() == static_cast<int>(Columns::Group))
        {
            return QString::fromStdString(_tocElements.at(index.row()).group);
        }
        if(index.column() == static_cast<int>(Columns::Name))
        {
            return QString::fromStdString(_tocElements.at(index.row()).name_only);
        }
        if(index.column() == static_cast<int>(Columns::Value))
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

Qt::ItemFlags ActualValuesModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::NoItemFlags;
}


