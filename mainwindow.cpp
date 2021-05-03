#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QStringList>
#include <QFileDialog>
#include <QStandardPaths>
#include "searchprodialog.h"
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /****************数据库初始化**********************/
    m_databaseMg=new databasemg();
    m_cusmodel=new QSqlTableModel(this,m_databaseMg->database());
    m_cusmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_cusmodel->setTable("customer");
    setCusHeaders();
    m_cusmodel->select();

    m_supmodel=new QSqlTableModel(this,m_databaseMg->database());
    m_supmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_supmodel->setTable("supplier");
    setSupHeaders();
    m_supmodel->select();

    m_promodel=new QSqlTableModel(this,m_databaseMg->database());
    m_promodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_promodel->setTable("product");
    setProHeaders();
    m_promodel->select();


    /*********ui布局初始化*******************/

    /*********1、客户管理界面*******************/
    ui->setupUi(this);
    //客户类型
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit(""));
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("企业"));
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("个人"));
    ui->tableViewCus->setModel(m_cusmodel);

    /*********2、供应商管理界面*******************/
    ui->comboBox_stype->addItem(QString::fromLocal8Bit(""));
    ui->comboBox_stype->addItem(QString::fromLocal8Bit("企业"));
    ui->comboBox_stype->addItem(QString::fromLocal8Bit("个人"));
    ui->tableViewSup->setModel(m_supmodel);

    /*********3、商品管理界面*******************/
    ui->tableViewPro->setModel(m_promodel);

    /*********4、销货管理界面*******************/
    m_sellSelectedNum.clear();
    updateSellCusComBox();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_cusmodel;
    delete m_databaseMg;
}


void MainWindow::on_groupBox_clicked()
{

}

void MainWindow::on_createcusBtn_clicked()
{
    QString name=ui->lineEdit_cname->text();
    if(name.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("客户姓名不能为空！"));
        return;
    }
    if(m_databaseMg->addCustomer(name,ui->comboBox_ctype->currentText(),ui->lineEdit_cpthone->text(),ui->textEdit_caddress->toPlainText()))
    {
        m_cusmodel->setTable("customer");
        setCusHeaders();
        m_cusmodel->select();
        ui->sell_customer_comboBox->addItem(name);
    }


    return;
}

void MainWindow::on_searchCusBtn_clicked()
{
    QString filter="";
    if(!ui->lineEdit_cname->text().isEmpty())
        filter+=QString("name like'%%1%'").arg(ui->lineEdit_cname->text());
    if(!ui->comboBox_ctype->currentText().isEmpty())
        if(filter.isEmpty())
            filter+=QString("type='%1'").arg(ui->comboBox_ctype->currentText());
        else
            filter+=QString(" and type='%1'").arg(ui->comboBox_ctype->currentText());
    if(!ui->lineEdit_cpthone->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("telephone like '%%1%'").arg(ui->lineEdit_cpthone->text());
        else
            filter+=QString(" and telephone like '%%1%'").arg(ui->lineEdit_cpthone->text());
     if(!ui->textEdit_caddress->toPlainText().isEmpty())
         if(filter.isEmpty())
             filter+=QString("address like '%%1%'").arg(ui->textEdit_caddress->toPlainText());
         else
             filter+=QString(" and address like '%%1%'").arg(ui->textEdit_caddress->toPlainText());
     if(filter.isEmpty())
     {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("搜索条件不能为空"));
     }
     else
     {
         m_cusmodel->setTable("customer");
         setCusHeaders();
         m_cusmodel->setFilter(filter);
         m_cusmodel->select();
     }
}

void MainWindow::on_showCusBtn_clicked()
{
    m_cusmodel->setTable("customer");
    setCusHeaders();
    m_cusmodel->select();
}

void MainWindow::on_inportCusBtn_clicked()
{
    QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),QString::fromLocal8Bit("表格文件(*.xls)"));
    if(m_databaseMg->importCustomersFromExcel(file))
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据成功。"));
        m_cusmodel->setTable("customer");
        setCusHeaders();
        m_cusmodel->select();
        updateSellCusComBox();
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据失败！"));
    }
}

void MainWindow::setCusHeaders()
{
    m_cusmodel->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("序号"));
    m_cusmodel->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("客户名称"));
    m_cusmodel->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("客户类型"));
    m_cusmodel->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("客户电话"));
    m_cusmodel->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("客户地址"));
}

void MainWindow::setSupHeaders()
{
    m_supmodel->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("序号"));
    m_supmodel->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("供应商名称"));
    m_supmodel->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("供应商类型"));
    m_supmodel->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("供应商电话"));
    m_supmodel->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("供应商地址"));
}

void MainWindow::setProHeaders()
{
    m_promodel->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("货品编号"));
    m_promodel->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("货品大类"));
    m_promodel->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("货品名称"));
    m_promodel->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("货品规格"));
    m_promodel->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("条码"));
    m_promodel->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("批号"));
    m_promodel->setHeaderData(6,Qt::Horizontal,QString::fromLocal8Bit("单位"));
    m_promodel->setHeaderData(7,Qt::Horizontal,QString::fromLocal8Bit("数量"));
    m_promodel->setHeaderData(8,Qt::Horizontal,QString::fromLocal8Bit("单价"));
    m_promodel->setHeaderData(9,Qt::Horizontal,QString::fromLocal8Bit("金额"));
    m_promodel->setHeaderData(10,Qt::Horizontal,QString::fromLocal8Bit("库存数量"));
    m_promodel->setHeaderData(11,Qt::Horizontal,QString::fromLocal8Bit("缺少数量"));
}

void MainWindow::updateSellCusComBox()
{
    ui->sell_customer_comboBox->addItem(QString::fromLocal8Bit(""));
    m_cusmodel->select();
    for(int i=0;i<m_cusmodel->rowCount();i++)
    {
        QSqlRecord rec=m_cusmodel->record(i);
        ui->sell_customer_comboBox->addItem(rec.value(1).toString());
    }
}

void MainWindow::on_createsupBtn_clicked()
{
    QString name=ui->lineEdit_sname->text();
    if(name.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("供应商姓名不能为空！"));
        return;
    }
    if(m_databaseMg->addSupplier(name,ui->comboBox_stype->currentText(),ui->lineEdit_sphone->text(),ui->textEdit_saddress->toPlainText()))
    {
        m_supmodel->setTable("supplier");
        setSupHeaders();
        m_supmodel->select();
    }
}

void MainWindow::on_searchSupBtn_clicked()
{
    QString filter="";
    if(!ui->lineEdit_sname->text().isEmpty())
        filter+=QString("name like'%%1%'").arg(ui->lineEdit_sname->text());
    if(!ui->comboBox_stype->currentText().isEmpty())
        if(filter.isEmpty())
            filter+=QString("type='%1'").arg(ui->comboBox_stype->currentText());
        else
            filter+=QString(" and type='%1'").arg(ui->comboBox_stype->currentText());
    if(!ui->lineEdit_sphone->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("telephone like '%%1%'").arg(ui->lineEdit_sphone->text());
        else
            filter+=QString(" and telephone like '%%1%'").arg(ui->lineEdit_sphone->text());
     if(!ui->textEdit_saddress->toPlainText().isEmpty())
         if(filter.isEmpty())
             filter+=QString("address like '%%1%'").arg(ui->textEdit_saddress->toPlainText());
         else
             filter+=QString(" and address like '%%1%'").arg(ui->textEdit_saddress->toPlainText());
     if(filter.isEmpty())
     {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("搜索条件不能为空"));
     }
     else
     {
         m_supmodel->setTable("supplier");
         setSupHeaders();
         m_supmodel->setFilter(filter);
         m_supmodel->select();
     }
}

void MainWindow::on_showSupBtn_clicked()
{
    m_supmodel->setTable("supplier");
    setSupHeaders();
    m_supmodel->select();
}

void MainWindow::on_inportSupBtn_clicked()
{
    QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),QString::fromLocal8Bit("表格文件(*.xls)"));
    if(m_databaseMg->importSuppliersFromExcel(file))
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据成功。"));
        m_supmodel->setTable("supplier");
        setSupHeaders();
        m_supmodel->select();
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据失败！"));
    }
}

void MainWindow::on_delCusBtn_clicked()
{
    QModelIndexList indexes=ui->tableViewCus->selectionModel()->selectedIndexes();
    QMap<int,int> rowMap;
    foreach(QModelIndex index,indexes)
        rowMap.insert(index.row(),index.row());
    if(rowMap.size()<=0)
    {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("没有行被选中！"));
         return;
    }
    int ok=QMessageBox::warning(this,QString::fromLocal8Bit("删除选中行！"),QString::fromLocal8Bit("你确定删除选中行吗？"),QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::Yes)
    {
       for(int i=0;i<rowMap.size();i++)
           m_cusmodel->removeRow(rowMap[i]);
        m_cusmodel->setTable("customer");
        setCusHeaders();
        m_cusmodel->select();
    }
}

void MainWindow::on_delSupBtn_clicked()
{
    QModelIndexList indexes=ui->tableViewSup->selectionModel()->selectedIndexes();

    QMap<int,int> rowMap;
    foreach(QModelIndex index,indexes)
        rowMap.insert(index.row(),index.row());
    if(rowMap.size()<=0)
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("没有行被选中！"));
        return;
    }

    int ok=QMessageBox::warning(this,QString::fromLocal8Bit("删除选中行！"),QString::fromLocal8Bit("你确定删除选中行吗？"),QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::Yes)
    {
        for(int i=0;i<rowMap.size();i++)
            m_supmodel->removeRow(rowMap[i]);
        m_supmodel->setTable("supplier");
        setSupHeaders();
        m_supmodel->select();
    }
}

void MainWindow::on_createpro_Btn_clicked()
{
    QString num=ui->pronum_lineEdit->text();
    if(num.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("商品编号不能为空！"));
        return;
    }
    QString name=ui->proname_lineEdit->text();
    if(name.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("商品名称不能为空！"));
        return;
    }
    QStringList list;
    list<<ui->pronum_lineEdit->text()<<ui->procategory_lineEdit->text()<<ui->proname_lineEdit->text()<<ui->prospec_lineEdit->text()
        <<ui->probarcode_lineEdit->text()<<ui->probatno_lineEdit->text()<<ui->prouint_lineEdit->text()
        <<ui->procount_spinBox->text()<<ui->proprice_spinBox->text()<<ui->prototalprice_spinBox->text()
        <<ui->prostack_spinBox->text()<<ui->prolack_spinBox->text();
    if(m_databaseMg->addProduct(list))
    {
        m_promodel->setTable("product");
        setProHeaders();
        m_promodel->select();
    }
}

void MainWindow::on_procount_spinBox_valueChanged(int arg1)
{
    int totalprice=ui->proprice_spinBox->value()*ui->procount_spinBox->value();
    ui->prototalprice_spinBox->setValue(totalprice);
}

void MainWindow::on_proprice_spinBox_valueChanged(int arg1)
{
    int totalprice=ui->proprice_spinBox->value()*ui->procount_spinBox->value();
    ui->prototalprice_spinBox->setValue(totalprice);
}

void MainWindow::on_inportProBtn_clicked()
{
    QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),QString::fromLocal8Bit("表格文件(*.xls)"));
    if(m_databaseMg->importProductsFromExcel(file))
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据成功。"));
        m_promodel->setTable("product");
        setProHeaders();
        m_promodel->select();
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据失败！"));
    }
}

void MainWindow::on_delProBtn_clicked()
{
    QModelIndexList indexes=ui->tableViewPro->selectionModel()->selectedIndexes();
    QMap<int,int> rowMap;
    foreach(QModelIndex index,indexes)
        rowMap.insert(index.row(),index.row());
    if(rowMap.size()<=0)
    {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("没有行被选中！"));
         return;
    }
    int ok=QMessageBox::warning(this,QString::fromLocal8Bit("删除选中行！"),QString::fromLocal8Bit("你确定删除选中行吗？"),QMessageBox::Yes,QMessageBox::No);
    if(ok==QMessageBox::Yes)
    {
       QMap<int, int>::const_iterator i;
       for (i = rowMap.constBegin(); i != rowMap.constEnd(); ++i)
          m_promodel->removeRow(i.value());
        m_promodel->setTable("product");
        setProHeaders();
        m_promodel->select();
    }
}

void MainWindow::on_searchProBtn_clicked()
{
    QString filter="";
    if(!ui->pronum_lineEdit->text().isEmpty())
        filter+=QString("number like'%%1%'").arg(ui->pronum_lineEdit->text());
    if(!ui->procategory_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("category like'%%1%'").arg(ui->procategory_lineEdit->text());
        else
            filter+=QString("and category like'%%1%'").arg(ui->procategory_lineEdit->text());
    if(!ui->proname_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("name like'%%1%'").arg(ui->proname_lineEdit->text());
        else
            filter+=QString("and name like'%%1%'").arg(ui->proname_lineEdit->text());
    if(!ui->prospec_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("spec like '%%1%'").arg(ui->prospec_lineEdit->text());
        else
            filter+=QString(" and spec like '%%1%'").arg(ui->prospec_lineEdit->text());
    if(!ui->probarcode_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("barcode like '%%1%'").arg(ui->probarcode_lineEdit->text());
        else
            filter+=QString(" and barcode like '%%1%'").arg(ui->probarcode_lineEdit->text());
     if(!ui->probatno_lineEdit->text().isEmpty())
         if(filter.isEmpty())
             filter+=QString("batchno like '%%1%'").arg(ui->probatno_lineEdit->text());
         else
             filter+=QString(" and batchno like '%%1%'").arg(ui->probatno_lineEdit->text());
     if(!ui->prouint_lineEdit->text().isEmpty())
          if(filter.isEmpty())
              filter+=QString("uint like '%%1%'").arg(ui->prouint_lineEdit->text());
          else
              filter+=QString(" and uint like '%%1%'").arg(ui->prouint_lineEdit->text());
     if(filter.isEmpty())
     {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("搜索条件不能为空"));
     }
     else
     {
         m_promodel->setTable("product");
         setProHeaders();
         m_promodel->setFilter(filter);
         m_promodel->select();
     }
}

void MainWindow::on_showProBtn_clicked()
{
    m_promodel->setTable("product");
    setProHeaders();
    m_promodel->select();
}

void MainWindow::on_sell_prono_lineEdit_textChanged(const QString &arg1)
{
   QSqlQuery query(m_databaseMg->database());
   if(query.exec(QString("select * from product where number is '%1'").arg(arg1)))
   {
       query.next();
       ui->sell_procat_lineEdit->setText(query.value(1).toString());
       ui->sell_proname_lineEdit->setText(query.value(2).toString());
       ui->sell_prospec_lineEdit->setText(query.value(3).toString());
       ui->sell_prostock_lineEdit->setText(query.value(10).toString());
   }
}

void MainWindow::on_pushButton_clicked()
{
    SearchProDialog* dlg=new SearchProDialog(this);
    dlg->setModal(true);
    if(dlg->exec()==QDialog::Accepted)
    {
        if(!m_sellSelectedNum.isEmpty())
           ui->sell_prono_lineEdit->setText(m_sellSelectedNum);
    }
}
