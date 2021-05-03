#include "databasemg.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextCodec>
#include <QMessageBox>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

databasemg::databasemg()
{
    createConnection();
    createTables();
}

bool databasemg::createConnection()
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("my.db");
    if(!m_db.open())
    {
        QMessageBox::critical(0,QString("打开数据库失败"),QString("无法建立数据库连接."),QMessageBox::Cancel);
        return false;
    }

    return true;
}

bool databasemg::createCustomerTable()
{
    QSqlQuery query(m_db);
    //创建客户表
    bool ret=query.exec("create table if not exists customer(id integer primary key autoincrement,name varchar(250),type varchar(32),telephone varchar(32),address varchar(256))");
    if(!ret)
    {
       QMessageBox::information(0,nullptr,QString::fromLocal8Bit("创建客户表失败"));
       qDebug()<<query.lastError();
       return false;
    }
    return true;
}

bool databasemg::addCustomer(QString name, QString type, QString pthone, QString address)
{
    QSqlQuery query(m_db);
    query.prepare("insert into customer (name,type,telephone,address) values (?,?,?,?)");
    query.addBindValue(name);
    query.addBindValue(type);
    query.addBindValue(pthone);
    query.addBindValue(address);
    bool ret=query.exec();
    if(ret)
     {
        QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建客户成功"));
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建客户失败！"));
        qDebug()<<query.lastError();
     }
    return ret;
}

bool databasemg::creatSupplierTable()
{
    QSqlQuery query(m_db);
    //创建供应商表
    bool ret=query.exec("create table if not exists supplier(id integer primary key autoincrement,name varchar(250),type varchar(32),telephone varchar(32),address varchar(256))");
    if(!ret)
    {
       QMessageBox::information(0,nullptr,QString::fromLocal8Bit("创建供应商表失败！"));
       qDebug()<<query.lastError();
       return false;
    }
    return true;
}

bool databasemg::addSupplier(QString name, QString type, QString pthone, QString address)
{
    QSqlQuery query(m_db);
    query.prepare("insert into supplier (name,type,telephone,address) values (?,?,?,?)");
    query.addBindValue(name);
    query.addBindValue(type);
    query.addBindValue(pthone);
    query.addBindValue(address);
    bool ret=query.exec();
    if(ret)
     {
        QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建供应商成功"));
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建供应商失败！"));
        qDebug()<<query.lastError();
     }
    return ret;
}

bool databasemg::createProductTable()
{
    QSqlQuery query(m_db);
    //创建产品表
    bool ret=query.exec("create table if not exists product(number varchar[16] primary key,category varchar[32],name varchar[32],spec varchar[32],barcode varchar[128],batchno varchar[16],unit varchar[8],count int,price int,totalprice int,stock int,lack int)");
    if(!ret)
    {
       QMessageBox::information(0,nullptr,QString::fromLocal8Bit("创建货品表失败！"));
       qDebug()<<query.lastError();
       return false;
    }
    return true;
}

bool databasemg::addProduct(QStringList list)
{
    if(list.size()<12)
    {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建供产品失败，缺少字段！"));
         return false;
    }
    QSqlQuery query(m_db);
    query.prepare("insert into product (number,category,name,spec,barcode,batchno,unit,count,price,totalprice,stock,lack) values (?,?,?,?,?,?,?,?,?,?,?,?)");
    for(int i=0;i<12;i++)
    {
        if(i<7)
         query.addBindValue(list[i]);
        else
         query.addBindValue(list[i].toInt());
    }
    bool ret=query.exec();
    if(ret)
     {
        QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建货品成功"));
    }
    else
    {
        if(query.lastError().text().contains("UNIQUE constraint failed: product.number"))
        {
             QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建货品%1失败，该编号已经存在，不允许有编号相同的货品").arg(list[0]));
        }
        else
        {
             QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建货品失败！"));
        }
        qDebug()<<query.lastError();
     }
    return ret;
}

bool databasemg::createSellTable()
{
    QSqlQuery query(m_db);
    //创建销货表
    query.exec("create table if not exists sell(id int primary key,category varchar,name varchar,spec varchar,foreign key(productno) reference product(number)),"
               "selldate date,sellnum int,unitprice int,totalprice int,haspay int,debt int,foreign key(customer) reference customer(name))");
    return true;
}

bool databasemg::createPurchaseTable()
{
    QSqlQuery query(m_db);
    //创建进货表
    query.exec("create table if not exists purchase(id int primary key,category varchar,name varchar,spec varchar,foreign key(productno) reference product(number)),"
               "purchase date,purchasenum int,unitprice int,totalprice int,haspay int,debt int,foreign key(supplier) reference product(name)),invoice char(1)");
    return true;
}

void databasemg::createTables()
{
    createCustomerTable();
    creatSupplierTable();
    createProductTable();
//    createSellTable();
    //    createPurchaseTable();
}


bool databasemg::importCustomersFromExcel(QString path)
{
    QList<QStringList> data;//所有记录
    if(getCustomersFromExcel(path,data))
    {
        return saveCustomers(data);
    }
    else
    {
        return false;
    }
}

bool databasemg::importSuppliersFromExcel(QString path)
{
    QList<QStringList> data;//所有记录
    if(getSuppliersFromExcel(path,data))
    {
        return saveSuppliers(data);
    }
    else
    {
        return false;
    }
}

bool databasemg::importProductsFromExcel(QString path)
{
    QList<QStringList> data;//所有记录
    if(getProductsFromExcel(path,data))
    {
        return saveProducts(data);
    }
    else
    {
        return false;
    }
}


bool databasemg::getCustomersFromExcel(QString path, QList<QStringList> &data)
{
    QSqlDatabase dbexcel = QSqlDatabase::addDatabase("QODBC", "excelexport");
    if(!dbexcel.isValid())
    {
         qDebug()<<"打开Excel失败";
         return false;
    }
    QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%1\";DBQ=%2").
                  arg(path).arg(path);
    dbexcel.setDatabaseName(dsn);

    if(!dbexcel.open())
    {
        qDebug()<<"打开Excel失败";
        return false;
    }

    QSqlQuery query(dbexcel);
    QStringList tables=dbexcel.tables();
    bool ret=query.exec("select * from [Sheet1$]");
    qDebug()<<ret;

    QStringList list;//一条记录
    while(query.next())
    {
        list.clear();
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString type = query.value(2).toString();
        QString pthone = query.value(3).toString();
        QString address = query.value(4).toString();
        list<<id<<name<<type<<pthone<<address;
        data<<list;
    }
    dbexcel.close();
    return true;
}

bool databasemg::saveCustomers(QList<QStringList> &data)
{
    QSqlQuery query(m_db);
    foreach(QStringList slist, data)
    {
        query.prepare("insert into customer(name,type,telephone,address) values (?,?,?,?)");
        //索引0是自增的，从1开始
        for(int i=1,n=slist.size();i<n;i++)
        {
            query.addBindValue(slist.at(i));
        }
        if(!query.exec()) {
            qDebug()<<"insert slist failed!"
                   <<"slist="
                   <<slist;
            return false;
        }

    }
    return true;
}

bool databasemg::getSuppliersFromExcel(QString path, QList<QStringList> &data)
{
    QSqlDatabase dbexcel = QSqlDatabase::addDatabase("QODBC", "excelexport");
    if(!dbexcel.isValid())
    {
         qDebug()<<"打开Excel失败";
         return false;
    }
    QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%1\";DBQ=%2").
                  arg(path).arg(path);
    dbexcel.setDatabaseName(dsn);

    if(!dbexcel.open())
    {
        qDebug()<<"打开Excel失败";
        return false;
    }

    QSqlQuery query(dbexcel);
    QStringList tables=dbexcel.tables();
    bool ret=query.exec("select * from [Sheet1$]");
    qDebug()<<ret;

    QStringList list;//一条记录
    while(query.next())
    {
        list.clear();
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString type = query.value(2).toString();
        QString pthone = query.value(3).toString();
        QString address = query.value(4).toString();
        list<<id<<name<<type<<pthone<<address;
        data<<list;
    }
    dbexcel.close();
    return true;
}

bool databasemg::saveSuppliers(QList<QStringList> &data)
{
    QSqlQuery query(m_db);
    foreach(QStringList slist, data)
    {
        query.prepare("insert into supplier(name,type,telephone,address) values (?,?,?,?)");
        //索引0是自增的，从1开始
        for(int i=1,n=slist.size();i<n;i++)
        {
            query.addBindValue(slist.at(i));
        }
        if(!query.exec()) {
            qDebug()<<"insert slist failed!"
                   <<"slist="
                   <<slist;
            return false;
        }

    }
    return true;
}

bool databasemg::getProductsFromExcel(QString path, QList<QStringList> &data)
{
    QSqlDatabase dbexcel = QSqlDatabase::addDatabase("QODBC", "excelexport");
    if(!dbexcel.isValid())
    {
         qDebug()<<"打开Excel失败";
         return false;
    }
    QString dsn = QString("DRIVER={Microsoft Excel Driver (*.xls, *.xlsx, *.xlsm, *.xlsb)};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%1\";DBQ=%2").
                  arg(path).arg(path);
    dbexcel.setDatabaseName(dsn);

    if(!dbexcel.open())
    {
        qDebug()<<"打开Excel失败";
        return false;
    }

    QSqlQuery query(dbexcel);
    QStringList tables=dbexcel.tables();
    bool ret=query.exec("select * from [Sheet1$]");
    qDebug()<<ret;

    QStringList list;//一条记录
    while(query.next())
    {
        list.clear();
        QString number = query.value(0).toString();
        QString category = query.value(1).toString();
        QString name = query.value(2).toString();
        QString spec = query.value(3).toString();
        QString barcode = query.value(4).toString();
        if(barcode=="0")
            barcode="";
        QString batchno = query.value(5).toString();
        QString unit = query.value(6).toString();
        QString count = query.value(7).toString();
        QString price = query.value(8).toString();
        QString totalprice = query.value(9).toString();
        QString stock = query.value(10).toString();
        QString lack = query.value(11).toString();
        list<<number<<category<<name<<spec<<barcode<<batchno<<unit<<count<<price<<totalprice<<stock<<lack;
        data<<list;
    }
    dbexcel.close();
    return true;
}

bool databasemg::saveProducts(QList<QStringList> &data)
{
    QSqlQuery query(m_db);
    foreach(QStringList slist, data)
    {
        query.prepare("insert into product (number,category,name,spec,barcode,batchno,unit,count,price,totalprice,stock,lack) "
                      "values (?,?,?,?,?,?,?,?,?,?,?,?)");
        for(int i=0,n=slist.size();i<n;i++)
        {
            if(i<7)
                query.addBindValue(slist.at(i));
            else
                query.addBindValue(slist.at(i).toInt());
        }
        if(!query.exec()) {
            qDebug()<<"insert slist failed!"
                   <<"slist="
                   <<slist;
            qDebug()<<query.lastError();
            if(query.lastError().text().contains("UNIQUE constraint failed: product.number"))
            {
                 QMessageBox::information(0,nullptr,QString::fromLocal8Bit("导入货品%1失败，该编号已经存在，不允许有编号相同的货品").arg(slist.at(0)));
            }
            return false;
        }

    }
    return true;
}
