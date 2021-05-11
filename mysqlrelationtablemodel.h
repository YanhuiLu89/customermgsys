#ifndef MYSQLRELATIONTABLEMODEL_H
#define MYSQLRELATIONTABLEMODEL_H

#include <QSqlRelationalTableModel>

class MySqlRelationTableModel : public QSqlRelationalTableModel
{
public:
    explicit MySqlRelationTableModel(QObject *parent = Q_NULLPTR,QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
};

#endif // MYSQLRELATIONTABLEMODEL_H
