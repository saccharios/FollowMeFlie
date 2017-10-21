#pragma once

#include "table_model_base.h"
#include <QObject>

class ParameterModel : public TableModelBase
{
    Q_OBJECT
    using Base = TableModelBase;

    static constexpr int cols = 5;
public:
    explicit ParameterModel(std::vector<TOCElement> const & tocElements, QObject *parent);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
//    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
//    Qt::ItemFlags flags(const QModelIndex& index) const override;

signals:
    void editCompleted(const QString &);
public slots:
    void UpdateParameter(uint8_t const & index);
};

