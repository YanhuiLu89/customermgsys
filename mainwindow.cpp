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
#include "mysqlrelationtablemodel.h"
#include <QSqlRelationalDelegate>
#include <QSqlField>
#include <QTextStream>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include "myprint.h"

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

    m_sellmodel=new MySqlRelationTableModel(this,m_databaseMg->database());
    m_sellmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_sellmodel->setTable("sell");
    m_sellmodel->setRelation(4,QSqlRelation("product","number","number"));
    m_sellmodel->setRelation(11,QSqlRelation("customer","name","name"));
    setSellHeaders();
    m_sellmodel->select();

    m_stockmodel=new MySqlRelationTableModel(this,m_databaseMg->database());
    m_stockmodel->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_stockmodel->setTable("stock");
    m_stockmodel->setRelation(4,QSqlRelation("product","number","number"));
    m_stockmodel->setRelation(11,QSqlRelation("supplier","name","name"));
    setStockHeaders();
    m_stockmodel->select();

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
    m_srchProSelectedNum.clear();
    ui->sell_date_dateEdit->setDate(QDate::currentDate());
    ui->sell_date_dateEdit->setDisplayFormat("yyyy-MM-dd");
    ui->sell_date_dateEdit->setCalendarPopup(true);
    QButtonGroup* gropRadionA=new QButtonGroup;
    gropRadionA->addButton(ui->tax_radioButton_yes,0);
    gropRadionA->addButton(ui->tax_radioButton_no,1);
    gropRadionA->setExclusive(true);
    QButtonGroup* gropRadionB=new QButtonGroup;
    gropRadionB->addButton(ui->invoice_radioButton_yes,0);
    gropRadionB->addButton(ui->invoice_radioButton_no,1);
    gropRadionB->setExclusive(true);
    updateSellCusComBox();
    ui->tableViewSell->setModel(m_sellmodel);
    ui->tableViewSell->setItemDelegate(new QSqlRelationalDelegate(ui->tableViewSell));

    /*********5、进货管理界面*******************/
    m_srchProSelectedNum.clear();
    ui->stock_date_dateEdit->setDate(QDate::currentDate());
    ui->stock_date_dateEdit->setDisplayFormat("yyyy-MM-dd");
    ui->stock_date_dateEdit->setCalendarPopup(true);
    updateStockSupComBox();
    ui->tableViewStock->setModel(m_stockmodel);
    ui->tableViewStock->setItemDelegate(new QSqlRelationalDelegate(ui->tableViewSell));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_cusmodel;
    delete m_supmodel;
    delete m_promodel;
    delete m_sellmodel;
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

void MainWindow::setSellHeaders()
{
    m_sellmodel->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("序号"));
    m_sellmodel->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("货品大类"));
    m_sellmodel->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("货品名称"));
    m_sellmodel->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("货品规格"));
    m_sellmodel->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("商品编码"));
    m_sellmodel->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("出货日期"));
    m_sellmodel->setHeaderData(6,Qt::Horizontal,QString::fromLocal8Bit("出货数量"));
    m_sellmodel->setHeaderData(7,Qt::Horizontal,QString::fromLocal8Bit("出货单价"));
    m_sellmodel->setHeaderData(8,Qt::Horizontal,QString::fromLocal8Bit("合计"));
    m_sellmodel->setHeaderData(9,Qt::Horizontal,QString::fromLocal8Bit("已付金额"));
    m_sellmodel->setHeaderData(10,Qt::Horizontal,QString::fromLocal8Bit("欠款"));
    m_sellmodel->setHeaderData(11,Qt::Horizontal,QString::fromLocal8Bit("客户"));
    m_sellmodel->setHeaderData(12,Qt::Horizontal,QString::fromLocal8Bit("是否含税"));
    m_sellmodel->setHeaderData(13,Qt::Horizontal,QString::fromLocal8Bit("是否欠发票"));
}

void MainWindow::setStockHeaders()
{
    m_stockmodel->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("序号"));
    m_stockmodel->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("货品大类"));
    m_stockmodel->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("货品名称"));
    m_stockmodel->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("货品规格"));
    m_stockmodel->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("商品编码"));
    m_stockmodel->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("进货日期"));
    m_stockmodel->setHeaderData(6,Qt::Horizontal,QString::fromLocal8Bit("进货数量"));
    m_stockmodel->setHeaderData(7,Qt::Horizontal,QString::fromLocal8Bit("进货单价"));
    m_stockmodel->setHeaderData(8,Qt::Horizontal,QString::fromLocal8Bit("合计"));
    m_stockmodel->setHeaderData(9,Qt::Horizontal,QString::fromLocal8Bit("已付金额"));
    m_stockmodel->setHeaderData(10,Qt::Horizontal,QString::fromLocal8Bit("欠款"));
    m_stockmodel->setHeaderData(11,Qt::Horizontal,QString::fromLocal8Bit("供应商"));
    m_stockmodel->setHeaderData(12,Qt::Horizontal,QString::fromLocal8Bit("是否开票"));
}

void MainWindow::updateSellCusComBox()
{
    ui->sell_customer_comboBox->clear();
    ui->sell_customer_comboBox->addItem(QString::fromLocal8Bit(""));
    m_cusmodel->select();
    for(int i=0;i<m_cusmodel->rowCount();i++)
    {
        QSqlRecord rec=m_cusmodel->record(i);
        ui->sell_customer_comboBox->addItem(rec.value(1).toString());
    }
}

void MainWindow::updateStockSupComBox()
{
    ui->stock_supplier_comboBox->clear();
    ui->stock_supplier_comboBox->addItem(QString::fromLocal8Bit(""));
    m_supmodel->select();
    for(int i=0;i<m_supmodel->rowCount();i++)
    {
        QSqlRecord rec=m_supmodel->record(i);
        ui->stock_supplier_comboBox->addItem(rec.value(1).toString());
    }
}

void MainWindow::exportSellTable(const QString &path)
{
   QStringList vList;
   for(int row=0;row<m_sellmodel->rowCount();row++)
   {
          QSqlRecord record=m_sellmodel->record(row);
          QString suffix=""; // 记录属性值
          // 遍历属性字段
          for(int i=0;i<record.count();i++)
          {
              QSqlField field=record.field(i);
              switch(field.type())
              {
              case QVariant::String:
                  suffix+=record.value(i).toString();
                  break;
              case QVariant::Int:
                  suffix+=QString("%1").arg(record.value(i).toInt());
                  break;
              case QVariant::Double:
                  suffix+=QString("%1").arg(record.value(i).toDouble());
                  break;
              case QVariant::Date:
                  suffix+=record.value(i).toString();
                  break;
            }
            if(i!=record.count()-1)
            {
                suffix+="\t";
            }
        }
          // 组装语句
          vList.append(suffix);
      }


      QFile file(path);
      if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
      {
          QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导出文件失败！"));
          return;
      }
      // 写入文件
      QTextStream out(&file);
      //写表头
      out<<QString::fromLocal8Bit("序号\t货品大类\t货品名称\t货品规格\t商品编码\t出货日期\t出货数量\t出货单价\t合计\t已付金额\t欠款\t客户\t是否含税\t是否欠发票")+"\n";
      //写内容
      foreach(QString line,vList)
      {
        out<<line+"\n";
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
        updateStockSupComBox();
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
        updateStockSupComBox();
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
        updateSellCusComBox();
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
        updateStockSupComBox();
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
       ui->sell_procat_showlabel->setText(query.value(1).toString());
       ui->sell_proname_showlabel->setText(query.value(2).toString());
       ui->sell_prospec_showlabel->setText(query.value(3).toString());
       ui->sell_prostock_showlabel->setText(query.value(10).toString());
   }
}

void MainWindow::on_pushButton_clicked()
{
    m_srchProSelectedNum.clear();
    SearchProDialog* dlg=new SearchProDialog(this);
    dlg->setModal(true);
    if(dlg->exec()==QDialog::Accepted)
    {
        if(!m_srchProSelectedNum.isEmpty())
           ui->sell_prono_lineEdit->setText(m_srchProSelectedNum);
    }
}

void MainWindow::on_sell_cnt_spinBox_valueChanged(int arg1)
{
    ui->sell_totalprice_doubleSpinBox->setValue(arg1*ui->sell_price_doubleSpinBox->value());
}

void MainWindow::on_sell_price_doubleSpinBox_valueChanged(double arg1)
{
    ui->sell_totalprice_doubleSpinBox->setValue(ui->sell_cnt_spinBox->value()*arg1);
}

void MainWindow::on_sell_totalprice_doubleSpinBox_valueChanged(double arg1)
{
    ui->sell_owed_doubleSpinBox->setValue(arg1-ui->sell_payed_doubleSpinBox->value());
}

void MainWindow::on_sell_payed_doubleSpinBox_valueChanged(double arg1)
{
    ui->sell_owed_doubleSpinBox->setValue(ui->sell_totalprice_doubleSpinBox->value()-arg1);
}

void MainWindow::on_sell_addBtn_clicked()
{
    int row=0;
    QString num=ui->sell_prono_lineEdit->text();
    if(num.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("商品编号不能为空！"));
        return;
    }
    QString cusName=ui->sell_customer_comboBox->currentText();
    if(cusName.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("客户不能为无效值！"));
        return;
    }
    if(m_databaseMg->addSellRecord(num,ui->sell_cnt_spinBox->value(),ui->sell_price_doubleSpinBox->value(),
                                   ui->sell_totalprice_doubleSpinBox->value(),ui->sell_payed_doubleSpinBox->value(),
                                   ui->sell_owed_doubleSpinBox->value(),cusName,ui->sell_date_dateEdit->date(),ui->tax_radioButton_yes->isChecked(),ui->invoice_radioButton_yes->isChecked()))
    {
        m_sellmodel->setTable("sell");
        m_sellmodel->setRelation(4,QSqlRelation("product","number","number"));
        m_sellmodel->setRelation(11,QSqlRelation("customer","name","name"));
        setSellHeaders();
        m_sellmodel->select();
    }
    return;
}

void MainWindow::on_sell_importBtn_clicked()
{
    QString file=QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("打开"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),QString::fromLocal8Bit("表格文件(*.xls)"));
    if(m_databaseMg->importSellRecsFromExcel(file))
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据成功。"));
        m_sellmodel->setTable("sell");
        m_sellmodel->setRelation(4,QSqlRelation("product","number","number"));
        m_sellmodel->setRelation(11,QSqlRelation("customer","name","name"));
        setSellHeaders();
        m_sellmodel->select();
    }
    else
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("导入数据失败！"));
    }
}

void MainWindow::on_sell_searchBtn_clicked()
{
    QString filter="";
    if(!ui->sell_prono_lineEdit->text().isEmpty())
        filter+=QString("productno like'%%1%'").arg(ui->sell_prono_lineEdit->text());
    if(ui->sell_date_checkBox->checkState()==Qt::Unchecked)
        if(filter.isEmpty())
            filter+=QString("selldate is'%1'").arg(ui->sell_date_dateEdit->text());
        else
            filter+=QString("and selldate is'%1'").arg(ui->sell_date_dateEdit->text());
    if(ui->sell_price_doubleSpinBox->value()>0)
        if(filter.isEmpty())
            filter+=QString("price is '%1'").arg(ui->sell_price_doubleSpinBox->value());
        else
            filter+=QString("and price is '%1'").arg(ui->sell_price_doubleSpinBox->value());
    if(ui->sell_cnt_spinBox->value()>0)
        if(filter.isEmpty())
            filter+=QString("cnt is '%1'").arg(ui->sell_cnt_spinBox->value());
        else
            filter+=QString(" and cnt is '%1'").arg(ui->sell_cnt_spinBox->value());
    if(ui->sell_totalprice_doubleSpinBox->value()>0)
        if(filter.isEmpty())
            filter+=QString("totalprice is '%1'").arg(ui->sell_totalprice_doubleSpinBox->value());
        else
            filter+=QString(" and totalprice is '%1'").arg(ui->sell_totalprice_doubleSpinBox->value());
     if(ui->sell_payed_doubleSpinBox->value()>0)
         if(filter.isEmpty())
             filter+=QString("payed is '%1'").arg(ui->sell_payed_doubleSpinBox->value());
         else
             filter+=QString(" and payed is '%1'").arg(ui->sell_payed_doubleSpinBox->value());
     if(ui->sell_owed_doubleSpinBox->value()>0)
          if(filter.isEmpty())
              filter+=QString("owned is '%1'").arg(ui->sell_owed_doubleSpinBox->value());
          else
              filter+=QString(" and owned like '%1'").arg(ui->sell_owed_doubleSpinBox->value());
     if(!ui->sell_customer_comboBox->currentText().isEmpty())
          if(filter.isEmpty())
              filter+=QString("customer is '%1'").arg(ui->sell_customer_comboBox->currentText());
          else
              filter+=QString(" and customer like '%1'").arg(ui->sell_customer_comboBox->currentText());
     if(ui->tax_checkBox->checkState()==Qt::Unchecked)
     {
         QString tax=ui->tax_radioButton_yes->isChecked()?
                     QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否");
         if(filter.isEmpty())
             filter+=QString("tax is'%1'").arg(tax);
         else
             filter+=QString("and tax is'%1'").arg(tax);
     }
     if(ui->invoice_checkBox->checkState()==Qt::Unchecked)
     {
         QString invoice=ui->invoice_radioButton_yes->isChecked()?
                     QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否");
         if(filter.isEmpty())
             filter+=QString("invoice is'%1'").arg(invoice);
         else
             filter+=QString("and invoice is'%1'").arg(invoice);
     }
     if(filter.isEmpty())
     {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("搜索条件不能为空"));
     }
     else
     {
         m_sellmodel->setTable("sell");
         m_sellmodel->setFilter(filter);
         setSellHeaders();
         m_sellmodel->setRelation(4,QSqlRelation("product","number","number"));
         m_sellmodel->setRelation(11,QSqlRelation("customer","name","name"));
         m_sellmodel->select();
     }
}

void MainWindow::on_sell_showallBtn_clicked()
{
    m_sellmodel->setTable("sell");
    setSellHeaders();
    m_sellmodel->setRelation(4,QSqlRelation("product","number","number"));
    m_sellmodel->setRelation(11,QSqlRelation("customer","name","name"));
    m_sellmodel->select();
}

void MainWindow::on_sell_exportBtn_clicked()
{
    QTime time =QTime::currentTime();
    QString file=QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("选择导出文件路径"),QString::fromLocal8Bit("sell_%1 %2-%3-%4.xls")
                                              .arg(QDate::currentDate().toString()).arg(time.hour()).arg(time.minute()).arg(time.second()),QString::fromLocal8Bit("表格文件(*.xls)"));
    if(!file.isEmpty())
        exportSellTable(file);
}

void MainWindow::on_sell_printBtn_clicked()
{
   int rowNum=m_sellmodel->rowCount();
   QString title1=QString("Student Information");//打印表格的标题
   QStringList columnList;//columnList为各列名
   std::vector<QStringList> dataLists;//dataLists为各行数据
   columnList<<QString::fromLocal8Bit("序号")<<QString::fromLocal8Bit("货品大类")<<QString::fromLocal8Bit("货品名称")
            <<QString::fromLocal8Bit("货品规格")<<QString::fromLocal8Bit("商品编码")<<QString::fromLocal8Bit("出货日期")
            <<QString::fromLocal8Bit("出货数量")<<QString::fromLocal8Bit("出货单价")<<QString::fromLocal8Bit("合计")
            <<QString::fromLocal8Bit("已付金额")<<QString::fromLocal8Bit("欠款")<<QString::fromLocal8Bit("客户")
            <<QString::fromLocal8Bit("是否含税")<<QString::fromLocal8Bit("是否欠发票");

   for(int i=0;i<rowNum;i++)
   {
       QStringList data;
       for(int j=0;j<14;j++)
       {
           data<<m_sellmodel->record(i).value(j).toString();
       }
       dataLists.push_back(data);
   }
   MyPrint myprint;
   myprint.dataBegin();
   myprint.tableBegin(columnList);
   for(int row=0;row<rowNum;row++)
   {
       myprint.insert2TableRow(dataLists[row]);
   }

   myprint.tableEnd();
   myprint.dataEnd();
   myprint.printWithPreview();
}

void MainWindow::on_sell_delBtn_clicked()
{
    QModelIndexList indexes=ui->tableViewSell->selectionModel()->selectedIndexes();
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
          m_sellmodel->removeRow(i.value());
        m_sellmodel->setTable("sell");
        setSellHeaders();
        m_sellmodel->setRelation(4,QSqlRelation("product","number","number"));
        m_sellmodel->setRelation(11,QSqlRelation("customer","name","name"));
        m_sellmodel->select();
    }
}

void MainWindow::on_tableViewSell_clicked(const QModelIndex &index)
{
    int column=index.column();
    if(column==4)//点击商品编号外键
    {
        QSqlRecord rec=m_sellmodel->record(index.row());
        QString pronumber=rec.value(4).toString();
        ui->tabWidget->setCurrentIndex(2);
        selectProduct(pronumber);

    }
    else if(column==11)//点击客户外键
    {
        QSqlRecord rec=m_sellmodel->record(index.row());
        QString name=rec.value(11).toString();
        ui->tabWidget->setCurrentIndex(0);
        selectCustomer(name);
    }
}

bool MainWindow::selectProduct(QString pronum)
{
    m_promodel->setTable("product");
    setProHeaders();
    m_promodel->setFilter(QString("number is '%1'").arg(pronum));
    m_promodel->select();
    return true;
}

bool MainWindow::selectCustomer(QString name)
{
    m_cusmodel->setTable("customer");
    setCusHeaders();
    m_cusmodel->setFilter(QString("name is '%1'").arg(name));
    m_cusmodel->select();
    return true;
}

void MainWindow::on_stock_prono_lineEdit_textChanged(const QString &arg1)
{
    QSqlQuery query(m_databaseMg->database());
    if(query.exec(QString("select * from product where number is '%1'").arg(arg1)))
    {
        query.next();
        ui->stock_procat_showlabel->setText(query.value(1).toString());
        ui->stock_proname_showlabel->setText(query.value(2).toString());
        ui->stock_prospec_showlabel->setText(query.value(3).toString());
        ui->stock_prostock_showlabel->setText(query.value(10).toString());
    }
}

void MainWindow::on_stock_pushButton_clicked()
{
    m_srchProSelectedNum.clear();
    SearchProDialog* dlg=new SearchProDialog(this);
    dlg->setModal(true);
    if(dlg->exec()==QDialog::Accepted)
    {
        if(!m_srchProSelectedNum.isEmpty())
           ui->stock_prono_lineEdit->setText(m_srchProSelectedNum);
    }
}

void MainWindow::on_stock_cnt_spinBox_valueChanged(int arg1)
{
    ui->stock_totalprice_doubleSpinBox->setValue(arg1*ui->stock_price_doubleSpinBox->value());
}

void MainWindow::on_stock_price_doubleSpinBox_valueChanged(double arg1)
{
    ui->stock_totalprice_doubleSpinBox->setValue(ui->stock_cnt_spinBox->value()*arg1);
}

void MainWindow::on_stock_totalprice_doubleSpinBox_valueChanged(double arg1)
{
    ui->stock_owed_doubleSpinBox->setValue(arg1-ui->stock_payed_doubleSpinBox->value());
}

void MainWindow::on_stock_payed_doubleSpinBox_valueChanged(double arg1)
{
    ui->stock_owed_doubleSpinBox->setValue(ui->stock_totalprice_doubleSpinBox->value()-arg1);
}

void MainWindow::on_stock_addBtn_clicked()
{
    int row=0;
    QString num=ui->stock_prono_lineEdit->text();
    if(num.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("商品编号不能为空！"));
        return;
    }
    QString supplierName=ui->stock_supplier_comboBox->currentText();
    if(supplierName.isEmpty())
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("供应商不能为无效值！"));
        return;
    }
    if(m_databaseMg->addStockRecord(num,ui->stock_cnt_spinBox->value(),ui->stock_price_doubleSpinBox->value(),
                                   ui->stock_totalprice_doubleSpinBox->value(),ui->stock_payed_doubleSpinBox->value(),
                                   ui->stock_owed_doubleSpinBox->value(),supplierName,ui->stock_date_dateEdit->date(),ui->stock_invoice_radioButton_yes->isChecked()))
    {
        m_stockmodel->setTable("stock");
        m_stockmodel->setRelation(4,QSqlRelation("product","number","number"));
        m_stockmodel->setRelation(11,QSqlRelation("supplier","name","name"));
        setStockHeaders();
        m_stockmodel->select();
    }
    return;
}

void MainWindow::on_stock_searchBtn_clicked()
{
    QString filter="";
    if(!ui->stock_prono_lineEdit->text().isEmpty())
        filter+=QString("productno like'%%1%'").arg(ui->stock_prono_lineEdit->text());
    if(ui->stock_date_checkBox->checkState()==Qt::Unchecked)
        if(filter.isEmpty())
            filter+=QString("stockdate is'%1'").arg(ui->stock_date_dateEdit->text());
        else
            filter+=QString(" and stockdate is'%1'").arg(ui->stock_date_dateEdit->text());
    if(ui->stock_price_doubleSpinBox->value()>0)
        if(filter.isEmpty())
            filter+=QString("price is '%1'").arg(ui->stock_price_doubleSpinBox->value());
        else
            filter+=QString(" and price is '%1'").arg(ui->stock_price_doubleSpinBox->value());
    if(ui->stock_cnt_spinBox->value()>0)
        if(filter.isEmpty())
            filter+=QString("cnt is '%1'").arg(ui->stock_cnt_spinBox->value());
        else
            filter+=QString(" and cnt is '%1'").arg(ui->stock_cnt_spinBox->value());
    if(ui->stock_totalprice_doubleSpinBox->value()>0)
        if(filter.isEmpty())
            filter+=QString("totalprice is '%1'").arg(ui->stock_totalprice_doubleSpinBox->value());
        else
            filter+=QString(" and totalprice is '%1'").arg(ui->stock_totalprice_doubleSpinBox->value());
     if(ui->stock_payed_doubleSpinBox->value()>0)
         if(filter.isEmpty())
             filter+=QString("payed is '%1'").arg(ui->stock_payed_doubleSpinBox->value());
         else
             filter+=QString(" and payed is '%1'").arg(ui->stock_payed_doubleSpinBox->value());
     if(ui->stock_owed_doubleSpinBox->value()>0)
          if(filter.isEmpty())
              filter+=QString("owned is '%1'").arg(ui->stock_owed_doubleSpinBox->value());
          else
              filter+=QString(" and owned like '%1'").arg(ui->stock_owed_doubleSpinBox->value());
     if(!ui->stock_supplier_comboBox->currentText().isEmpty())
          if(filter.isEmpty())
              filter+=QString("supplier is '%1'").arg(ui->stock_supplier_comboBox->currentText());
          else
              filter+=QString(" and supplier like '%1'").arg(ui->stock_supplier_comboBox->currentText());
     if(ui->stock_invoice_checkBox->checkState()==Qt::Unchecked)
     {
         QString invoice=ui->stock_invoice_radioButton_yes->isChecked()?
                     QString::fromLocal8Bit("是"):QString::fromLocal8Bit("否");
         if(filter.isEmpty())
             filter+=QString("invoice is'%1'").arg(invoice);
         else
             filter+=QString(" and invoice is'%1'").arg(invoice);
     }
     if(filter.isEmpty())
     {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("搜索条件不能为空"));
     }
     else
     {
         m_stockmodel->setTable("stock");
         m_stockmodel->setFilter(filter);
         setStockHeaders();
         m_stockmodel->setRelation(4,QSqlRelation("product","number","number"));
         m_stockmodel->setRelation(11,QSqlRelation("supplier","name","name"));
         m_stockmodel->select();
     }
}
