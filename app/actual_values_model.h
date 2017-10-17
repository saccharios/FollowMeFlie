#pragma once

#include "table_model_base.h"
#include <QObject>

class ActualValuesModel : public TableModelBase
{
    Q_OBJECT
    using Base = TableModelBase;
    static constexpr int cols = 4;
public:
    explicit ActualValuesModel(std::vector<TOCElement> const & tocElements, QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    public slots:
    void UpdateActualValues();

};
