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
    if(QSqlDatabase::contains("qt_sql_default_connection"))
      m_db = QSqlDatabase::database("qt_sql_default_connection");
    else
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

    bool ret= query.exec("create table if not exists sell("
                         "id integer primary key autoincrement,category varchar(32),name varchar(32),spec varchar(32),productno varchar(16),"
                         "selldate date,cnt int,price int,totalprice int,payed int,owned int,customer varchar(250),tax varchar(4),invoice varchar(4),"
                         "foreign key(customer) references customer(name),foreign key(productno) references product(number)"
                         ")");
    if(!ret)
    {
       QMessageBox::information(0,nullptr,QString::fromLocal8Bit("创建销货表失败！"));
       qDebug()<<query.lastError();
        return false;
    }
    return true;
}

bool databasemg::addSellRecord(QString pronum, int cnt, int price, int totalprice, int payed, int owned, QString customerName, QDate date,bool tax,bool invoice)
{
    QSqlQuery queryPro(m_db);
    if(queryPro.exec(QString("select * from product where number is '%1'").arg(pronum)))
    {
        queryPro.next();
        QString category=queryPro.value(1).toString();
        QString name=queryPro.value(2).toString();
        QString spec=queryPro.value(3).toString();
        int stock=queryPro.value(10).toInt();
        QSqlQuery query(m_db);
        query.prepare("insert into sell (category,name,spec,productno,selldate,cnt,price,totalprice,payed,owned,customer,tax,invoice) values (?,?,?,?,?,?,?,?,?,?,?,?,?)");
        query.addBindValue(category);
        query.addBindValue(name);
        query.addBindValue(spec);
        query.addBindValue(pronum);
        query.addBindValue(date);
        query.addBindValue(cnt);
        query.addBindValue(price);
        query.addBindValue(totalprice);
        query.addBindValue(payed);
        query.addBindValue(owned);
        query.addBindValue(customerName);
        if(tax)
            query.addBindValue(QString::fromLocal8Bit("是"));
        else
            query.addBindValue(QString::fromLocal8Bit("否"));
        if(invoice)
            query.addBindValue(QString::fromLocal8Bit("是"));
        else
            query.addBindValue(QString::fromLocal8Bit("否"));
        bool ret=query.exec();
        if(ret)
         {
            QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建销货记录成功"));
            //更新库存
            queryPro.exec(QString("update product set stock=%1 where number is '%2'").arg(stock-cnt).arg(pronum));
            return true;
        }
        else
        {
            QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建销货记录失败！"));
            qDebug()<<query.lastError();
            return false;
         }
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建销货记录失败！"));
        qDebug()<<queryPro.lastError();
        return false;
    }

}

bool databasemg::createStockTable()
{

    QSqlQuery query(m_db);
    //创建进货表

    bool ret= query.exec("create table if not exists stock("
                         "id integer primary key autoincrement,category varchar(32),name varchar(32),spec varchar(32),productno varchar(16),"
                         "stockdate date,cnt int,price int,totalprice int,payed int,owned int,supplier varchar(250),invoice varchar(4),"
                         "foreign key(supplier) references supplier(name),foreign key(productno) references product(number)"
                         ")");
    if(!ret)
    {
       QMessageBox::information(0,nullptr,QString::fromLocal8Bit("创建进货表失败！"));
       qDebug()<<query.lastError();
        return false;
    }
}

bool databasemg::addStockRecord(QString pronum, int cnt, int price, int totalprice, int payed, int owned, QString supplierName, QDate date, bool invoice)
{
    QSqlQuery queryPro(m_db);
    if(queryPro.exec(QString("select * from product where number is '%1'").arg(pronum)))
    {
        queryPro.next();
        QString category=queryPro.value(1).toString();
        QString name=queryPro.value(2).toString();
        QString spec=queryPro.value(3).toString();
        int stock=queryPro.value(10).toInt();
        QSqlQuery query(m_db);
        query.prepare("insert into stock (category,name,spec,productno,stockdate,cnt,price,totalprice,payed,owned,supplier,invoice) values (?,?,?,?,?,?,?,?,?,?,?,?)");
        query.addBindValue(category);
        query.addBindValue(name);
        query.addBindValue(spec);
        query.addBindValue(pronum);
        query.addBindValue(date);
        query.addBindValue(cnt);
        query.addBindValue(price);
        query.addBindValue(totalprice);
        query.addBindValue(payed);
        query.addBindValue(owned);
        query.addBindValue(supplierName);
        if(invoice)
            query.addBindValue(QString::fromLocal8Bit("是"));
        else
            query.addBindValue(QString::fromLocal8Bit("否"));
        bool ret=query.exec();
        if(ret)
         {
            QMessageBox::information(0,nullptr,QString::fromLocal8Bit("新建销货记录成功"));
            //更新库存
            queryPro.exec(QString("update product set stock=%1 where number is '%2'").arg(stock+cnt).arg(pronum));
            return true;
        }
        else
        {
            QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建销货记录失败！"));
            qDebug()<<query.lastError();
            return false;
         }
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("新建销货记录失败！"));
        qDebug()<<queryPro.lastError();
        return false;
    }

}

void databasemg::createTables()
{
    createCustomerTable();
    creatSupplierTable();
    createProductTable();
    createSellTable();
    createStockTable();
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

bool databasemg::importSellRecsFromExcel(QString path)
{
    QList<QStringList> data;//所有记录
    if(getSellRecsFromExcel(path,data))
    {
        return saveSellRecs(data);
    }
    else
    {
        return false;
    }
}

bool databasemg::importStockRecsFromExcel(QString path)
{
    QList<QStringList> data;//所有记录
    if(getStockRecsFromExcel(path,data))
    {
        return saveStockRecs(data);
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
    query.exec("select * from [Sheet1$]");

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

bool databasemg::getSellRecsFromExcel(QString path, QList<QStringList> &data)
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
    query.exec("select * from [Sheet1$]");

    QStringList list;//一条记录
    while(query.next())
    {
        list.clear();
        QString category = query.value(1).toString();
        QString name = query.value(2).toString();
        QString spec = query.value(3).toString();
        QString productno = query.value(4).toString();
        QString selldate = query.value(5).toString();
        QString cnt = query.value(6).toString();
        QString price = query.value(7).toString();
        QString totalprice = query.value(8).toString();
        QString payed = query.value(9).toString();
        QString owned = query.value(10).toString();
        QString customer = query.value(11).toString();
        QString tax=query.value(12).toString();
        QString invoice=query.value(13).toString();
        list<<category<<name<<spec<<productno<<selldate<<cnt<<price<<totalprice
           <<payed<<owned<<customer<<tax<<invoice;
        data<<list;
    }
    dbexcel.close();
    return true;
}

bool databasemg::saveSellRecs(QList<QStringList> &data)
{
    QSqlQuery query(m_db);
    foreach(QStringList slist, data)
    {
        query.prepare("insert into sell (category,name,spec,productno,selldate,cnt,price,totalprice,payed,owned,customer,tax,invoice) values (?,?,?,?,?,?,?,?,?,?,?,?,?)");
        for(int i=0,n=slist.size();i<n;i++)
        {
            if(i<4)
                query.addBindValue(slist.at(i));
            else if(i==4)
            {
               QString str=slist.at(i).mid(0,10);
               QDate date=QDate::fromString(str,"yyyy-MM-dd");
               query.addBindValue(date);
            }
            else if(i<10)
                query.addBindValue(slist.at(i).toInt());
            else
                query.addBindValue(slist.at(i));
        }
        if(!query.exec()) {
            qDebug()<<"insert slist failed!"
                   <<"slist="
                   <<slist;
            qDebug()<<query.lastError();
            QMessageBox::information(0,nullptr,QString::fromLocal8Bit("导入销货表失败"));
            return false;
        }
        //更新库存
        QSqlQuery queryPro(m_db);
        queryPro.exec(QString("select * from product where number is '%1'").arg(slist.at(3)));
        queryPro.next();
        int stock=queryPro.value(10).toInt();
        queryPro.exec(QString("update product set stock=%1 where number is '%2'").arg(stock-slist.at(5).toInt()).arg(slist.at(3)));

    }
    return true;
}

bool databasemg::getStockRecsFromExcel(QString path, QList<QStringList> &data)
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
    query.exec("select * from [Sheet1$]");

    QStringList list;//一条记录
    while(query.next())
    {
        list.clear();
        QString category = query.value(1).toString();
        QString name = query.value(2).toString();
        QString spec = query.value(3).toString();
        QString productno = query.value(4).toString();
        QString stockdate = query.value(5).toString();
        QString cnt = query.value(6).toString();
        QString price = query.value(7).toString();
        QString totalprice = query.value(8).toString();
        QString payed = query.value(9).toString();
        QString owned = query.value(10).toString();
        QString supplier = query.value(11).toString();
        QString invoice=query.value(12).toString();
        list<<category<<name<<spec<<productno<<stockdate<<cnt<<price<<totalprice
           <<payed<<owned<<supplier<<invoice;
        data<<list;
    }
    dbexcel.close();
    return true;
}

bool databasemg::saveStockRecs(QList<QStringList> &data)
{
    QSqlQuery query(m_db);
    foreach(QStringList slist, data)
    {
        query.prepare("insert into stock (category,name,spec,productno,stockdate,cnt,price,totalprice,payed,owned,supplier,invoice) values (?,?,?,?,?,?,?,?,?,?,?,?)");
        for(int i=0,n=slist.size();i<n;i++)
        {
            if(i<4)
                query.addBindValue(slist.at(i));
            else if(i==4)
            {
               QString str=slist.at(i).mid(0,10);
               QDate date=QDate::fromString(str,"yyyy-MM-dd");
               query.addBindValue(date);
            }
            else if(i==5)
                query.addBindValue(slist.at(i).toInt());
            else if(i<10)
                query.addBindValue(slist.at(i).toInt());
            else
                query.addBindValue(slist.at(i));
        }
        if(!query.exec()) {
            qDebug()<<"insert slist failed!"
                   <<"slist="
                   <<slist;
            qDebug()<<query.lastError();
            QMessageBox::information(0,nullptr,QString::fromLocal8Bit("导入进货表失败"));
            return false;
        }
        //更新库存
        QSqlQuery queryPro(m_db);
        queryPro.exec(QString("select * from product where number is '%1'").arg(slist.at(3)));
        queryPro.next();
        int stock=queryPro.value(10).toInt();
        queryPro.exec(QString("update product set stock=%1 where number is '%2'").arg(stock+slist.at(5).toInt()).arg(slist.at(3)));

    }
    return true;
}
