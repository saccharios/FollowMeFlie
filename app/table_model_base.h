#pragma once
#include <QAbstractTableModel>
#include "math/types.h"

class TableModelBase : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModelBase(std::vector<TOCElement> const & tocElements, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    // Header:
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override = 0;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0 ;

        // Add data:
        bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

        // Remove data:
        bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    //    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;


protected:
    enum class Columns
    {
        Index = 0,
        Group = 1,
        Name = 2,
        Value = 3,
        Edit = 4
    };
    std::vector<TOCElement> const & _tocElements;
    std::map<Columns, QString>  _header;

private:

};
