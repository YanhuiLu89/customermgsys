#ifndef DATABASEMG_H
#define DATABASEMG_H
#include <QSqlDatabase>

class databasemg
{
public:
    databasemg();
    bool createConnection();
    bool createCustomerTable();
    bool creatSupplierTable();
    bool createProductTable();
    bool createSellTable();
    bool createPurchaseTable();
    void createTables();
private:
    QSqlDatabase m_db;
};

#endif // DATABASEMG_H
