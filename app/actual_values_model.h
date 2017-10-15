#pragma once

#include "table_model_base.h"
#include <QObject>

class ActualValuesModel : public TableModelBase
{
    Q_OBJECT
    using Base = TableModelBase;
    static constexpr int rows = 2;
    static constexpr int cols = 4;
public:
    explicit ActualValuesModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
};
