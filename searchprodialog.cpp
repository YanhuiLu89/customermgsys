#include "searchprodialog.h"
#include "ui_searchprodialog.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include "mainwindow.h"
#include <QSqlRecord>

SearchProDialog::SearchProDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchProDialog)
{
    ui->setupUi(this);
    m_databaseMg=new databasemg();
    m_promodel=new QSqlTableModel(this,m_databaseMg->database());
    m_promodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_promodel->setTable("product");
    setProHeaders();
    ui->searchTableViewPro->setModel(m_promodel);
}

SearchProDialog::~SearchProDialog()
{
    delete ui;
}

void SearchProDialog::on_searchProBtn_2_clicked()
{
    QString filter="";
    if(!ui->search_pronum_lineEdit->text().isEmpty())
        filter+=QString("number like'%%1%'").arg(ui->search_pronum_lineEdit->text());
    if(!ui->search_procategory_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("category like'%%1%'").arg(ui->search_procategory_lineEdit->text());
        else
            filter+=QString("and category like'%%1%'").arg(ui->search_procategory_lineEdit->text());
    if(!ui->search_proname_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("name like'%%1%'").arg(ui->search_proname_lineEdit->text());
        else
            filter+=QString("and name like'%%1%'").arg(ui->search_proname_lineEdit->text());
    if(!ui->search_prospec_lineEdit->text().isEmpty())
        if(filter.isEmpty())
            filter+=QString("spec like'%1'").arg(ui->search_prospec_lineEdit->text());
        else
            filter+=QString(" and spec like'%1'").arg(ui->search_prospec_lineEdit->text());

     if(!ui->search_probatno_lineEdit->text().isEmpty())
         if(filter.isEmpty())
             filter+=QString("batchno like '%%1%'").arg(ui->search_probatno_lineEdit->text());
         else
             filter+=QString(" and batchno like '%%1%'").arg(ui->search_probatno_lineEdit->text());
     if(ui->search_prostack_spinBox->value()>0)
          if(filter.isEmpty())
              filter+=QString("stock >= %1 ").arg(ui->search_prostack_spinBox->value());
          else
              filter+=QString(" and stock >= %1 ").arg(ui->search_prostack_spinBox->value());
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

void SearchProDialog::accept()
{
    QModelIndexList indexes=ui->searchTableViewPro->selectionModel()->selectedIndexes();
    QMap<int,int> rowMap;
    foreach(QModelIndex index,indexes)
        rowMap.insert(index.row(),index.row());
    if(rowMap.size()<=0)
    {
         QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("没有行被选中！"));
         return;
    }
    else if(rowMap.size()>1)
    {
        QMessageBox::warning(0,nullptr,QString::fromLocal8Bit("只能选中一条！"));
        return;
    }
     QSqlRecord selected=m_promodel->record(rowMap.first());
    ((MainWindow*)parent())->setSellSelectedNum(selected.value(0).toString());
    return QDialog::accept();
}

void SearchProDialog::setProHeaders()
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
