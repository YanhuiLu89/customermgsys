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
    bool addSellRecord(QString pronum,int cnt,int price,int totalprice,int payed,int owned,QString customerName,QDate date,bool tax,bool invoice);

    bool createStockTable();
    bool addStockRecord(QString pronum,int cnt,int price,int totalprice,int payed,int owned,QString supplierName,QDate date,bool invoice);

    void createTables();

    bool importCustomersFromExcel(QString path);
    bool importSuppliersFromExcel(QString path);
    bool importProductsFromExcel(QString path);
    bool importSellRecsFromExcel(QString path);
    bool importStockRecsFromExcel(QString path);
private:
    bool getCustomersFromExcel(QString path,QList<QStringList>& data);
    bool saveCustomers(QList<QStringList>& data);

    bool getSuppliersFromExcel(QString path,QList<QStringList>& data);
    bool saveSuppliers(QList<QStringList>& data);

    bool getProductsFromExcel(QString path,QList<QStringList>& data);
    bool saveProducts(QList<QStringList>& data);

    bool getSellRecsFromExcel(QString path,QList<QStringList>& data);
    bool saveSellRecs(QList<QStringList>& data);

    bool getStockRecsFromExcel(QString path,QList<QStringList>& data);
    bool saveStockRecs(QList<QStringList>& data);


private:
    QSqlDatabase m_db;

};

#endif // DATABASEMG_H
