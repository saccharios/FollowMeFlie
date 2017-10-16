#include "actual_values_model.h"

ActualValuesModel::ActualValuesModel(std::vector<TOCElement> const & tocElements, QObject *parent):
    Base(tocElements, parent)
{}


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

