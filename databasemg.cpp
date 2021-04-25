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
       QMessageBox::critical(0,"Cannot open database","Unable to establish a datavase connection.",QMessageBox::Cancel);
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
    query.exec("create table if not exists supplier(id int primary key,name varchar,type varchar,telephone varchar,address varchar)");
    return true;
}

bool databasemg::createProductTable()
{
    QSqlQuery query(m_db);
    //创建产品表
    query.exec("create table if not exists product(id int primary key,number varchar,name varchar,spec varchar,barcode varchar,batchnum varchar,unit varcher,count int,price int,totalprice int)");
    return true;
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
