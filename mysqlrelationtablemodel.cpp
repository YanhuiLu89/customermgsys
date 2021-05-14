#include "mysqlrelationtablemodel.h"
#include <QFont>
#include <QColor>

MySqlRelationTableModel::MySqlRelationTableModel(QObject *parent, QSqlDatabase db)
    :QSqlRelationalTableModel(parent,db)
{

}

QVariant MySqlRelationTableModel::data(const QModelIndex &idx, int role) const
{
    QVariant value  = QSqlTableModel::data(idx,role);
    if(Qt::ForegroundRole == role)
    {
        if(4 == idx.column()||11 == idx.column())
            return QVariant(QColor(0,0,255));
    }
    if(Qt::FontRole == role)
    {
        if(4 == idx.column()||11 == idx.column())
        {
            QFont font;
            font.setUnderline(true);
            return QVariant(font);
        }
    }
    return value;
}

Qt::ItemFlags MySqlRelationTableModel::flags(const QModelIndex &index) const
{
    if(index.column()<4)
    {
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable ;
    }
    else
    {
       return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
    }
}
