#include "parameter_model.h"


ParameterModel::ParameterModel(std::vector<TOCElement> const & tocElements, QObject *parent):
    Base(tocElements, parent)
{
    // Is already added by the bas class
//    _header[Columns::Index] = QString("Index");
//    _header[Columns::Group] = QString("Group");
//    _header[Columns::Name] = QString("Name");
//    _header[Columns::Value] = QString("Value");
    _header[Columns::Edit] = QString("Edit Value");
}

QVariant ParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
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

int ParameterModel::rowCount(const QModelIndex & /*parent*/) const
{
    return rows;
}

int ParameterModel::columnCount(const QModelIndex & /*parent*/) const
{
    return cols;
}

QVariant ParameterModel::data(const QModelIndex &index, int role) const
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
        if(index.column() == static_cast<int>(Columns::Edit))
        {
//            return _values[index.row()];
        }
    }
    return QVariant();
}



bool ParameterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{

    if(index.column() == static_cast<int>(Columns::Edit))
    {
        if (role == Qt::EditRole)
        {
//            _values[index.row()] = value.toString();
//            emit editCompleted( _values[index.row()] );
        }
    }
    return true;

}

Qt::ItemFlags ParameterModel::flags(const QModelIndex &index) const
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

