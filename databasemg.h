#ifndef DATABASEMG_H
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
    bool createProductTable();
    bool createSellTable();
    bool createPurchaseTable();
    void createTables();
private:
    QSqlDatabase m_db;

};

#endif // DATABASEMG_H
