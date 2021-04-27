﻿#ifndef DATABASEMG_H
#define DATABASEMG_H
#include <QSqlDatabase>

class databasemg
{
public:
    databasemg();
    QSqlDatabase database(){return m_db;}
    bool createConnection();
    bool createCustomerTable();
    bool addCustomer(QString name,QString type,QString pthone,QString address);
    bool creatSupplierTable();
    bool addSupplier(QString name,QString type,QString pthone,QString address);
    bool createProductTable();
    bool createSellTable();
    bool createPurchaseTable();
    void createTables();

    bool importCustomersFromExcel(QString path);
    bool importSuppliersFromExcel(QString path);
private:
    bool getCustomersFromExcel(QString path,QList<QStringList>& data);
    bool saveCustomers(QList<QStringList>& data);

    bool getSuppliersFromExcel(QString path,QList<QStringList>& data);
    bool saveSuppliers(QList<QStringList>& data);

private:
    QSqlDatabase m_db;

};

#endif // DATABASEMG_H
