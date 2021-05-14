#ifndef MYSQLRELATIONTABLEMODEL_H
#define MYSQLRELATIONTABLEMODEL_H

#include <QSqlRelationalTableModel>

class MySqlRelationTableModel : public QSqlRelationalTableModel
{
public:
    explicit MySqlRelationTableModel(QObject *parent = Q_NULLPTR,QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // MYSQLRELATIONTABLEMODEL_H
