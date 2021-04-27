#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QStringList>
#include <QFileDialog>
#include <QStandardPaths>

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

    /*********ui布局初始化*******************/
    ui->setupUi(this);
    //客户类型
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit(""));
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("企业"));
    ui->comboBox_ctype->addItem(QString::fromLocal8Bit("个人"));
    ui->tableView->setModel(m_cusmodel);
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
