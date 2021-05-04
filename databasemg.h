#ifndef DATABASEMG_H
#define DATABASEMG_H
#include <QSqlDatabase>
#include <QDate>

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
    bool addProduct(QStringList list);

    bool createSellTable();
    bool addSellRecord(QString pronum,int cnt,double price,double totalprice,double payed,double owned,int cusindex,QDate date);

    bool createPurchaseTable();
    void createTables();

    bool importCustomersFromExcel(QString path);
    bool importSuppliersFromExcel(QString path);
    bool importProductsFromExcel(QString path);
private:
    bool getCustomersFromExcel(QString path,QList<QStringList>& data);
    bool saveCustomers(QList<QStringList>& data);

    bool getSuppliersFromExcel(QString path,QList<QStringList>& data);
    bool saveSuppliers(QList<QStringList>& data);

    bool getProductsFromExcel(QString path,QList<QStringList>& data);
    bool saveProducts(QList<QStringList>& data);

private:
    QSqlDatabase m_db;

};

#endif // DATABASEMG_H
