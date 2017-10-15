#ifndef ACTUALVALUESMODEL_H
#define ACTUALVALUESMODEL_H

#include <QAbstractTableModel>

class ActualValuesModel : public QAbstractTableModel
{
    Q_OBJECT

    enum class Columns
    {
        Index = 0,
        Group = 1,
        Name = 2,
        Value = 3,
        Edit = 4,
        Count = 5
    };

    static constexpr int rows = 2;
public:
    explicit ActualValuesModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

//    // Add data:
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

//    // Remove data:
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
//    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    std::array<QString,rows> _values;
    std::map<Columns, QString>  _header;

signals:
    void editCompleted(const QString &);
};

#endif // ACTUALVALUESMODEL_H
