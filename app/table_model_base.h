#ifndef TABLEMODELBASE_H
#define TABLEMODELBASE_H

#include <QAbstractItemModel>

class TableModelBase : public QAbstractItemModel
{
    Q_OBJECT
    enum class Columns
    {
        Index = 0,
        Group = 1,
        Name = 2,
        Value = 3,
        Edit = 4
    };
    static constexpr int rows = 2;
    static constexpr int cols = 4;
public:
    explicit TableModelBase(QObject *parent = 0);

    // Basic functionality:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::array<QString,rows> _values;
    std::map<Columns, QString>  _header;
};

#endif // TABLEMODELBASE_H
